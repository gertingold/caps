#include <stddef.h>
#include <assert.h>
#include <math.h>

#include "floattypes.h"
#include "integration_drude.h"
#include "libcasimir.h"
#include "utils.h"
#include "sfunc.h"
#include "plm_cache.h"

/*
 * The cache will save the values up to "max_iteration"
 */
#define max_iteration 20

/*
 * The cache will not save the values below "min_iteration"
 */
#define min_iteration 3

/*
 * Don't use this cache if the mean value of l1 and l2 is smaller than min_l
 */
#define min_l         2


/*
 * Count the number of cache-hits and cache-misses.
 * This is only useful for debugging and performance testing.
 */
//#define CACHE_STATS


/*
 * Debug this file
 */
//#define DEBUG_PLM_CACHE


enum cache_flags
{
    /*
     * The value of this entry is valid
     */
    CACHE_FLAG_VALID = 1U,
};


/*
 * This structure stores the legendre polynoms for a specific
 * value of l1, l2, m.
 */
struct cache_entry
{
    unsigned int      l1, l2;
    
    float80           lnPl1;
    sign_t            sign_Pl1;

    float80           lnPl2;
    sign_t            sign_Pl2;

    float80           lnPl1p1;
    sign_t            sign_Pl1p1;

    float80           lnPl2p1;
    sign_t            sign_Pl2p1;
    
    sign_t            common_sign;
};


struct cache_iteration
{
    /*
     * Dynamically allocated memory for the cache_entries.
     * Iteration "i" should have 2^i cache_entries.
     */
    struct cache_entry* entry;
    
    /*
     * Flags from "enum cache_flags"
     */
    unsigned char       flags;
};


struct cache_values
{
    /*
     * We don't use the lower iterations, but we have a full array of cache_iterations
     * in memory, because this makes the indexing easier. And it doesn't take much
     * memory since we don't allocate space for the "struct cache_entry"s.
     */
    struct cache_iteration iterations[max_iteration + 1];
};


struct plm_cache
{
    casimir_t* casimir;
    /*
     * The value for m. A cache can only save the values for one
     * m at once.
     */
    int m;

    /*
     * l1 + l2. We can reuse the previous results of plm if l1pl2 is kept constant
     */
    int l1pl2;
    
    /*
     * Matsubara term. Each thread should build up one object
     * of struct plm_cache
     */
    int n;

    /*
     * Values of the legendre polynomials of the last successful cache access.
     */
    struct cache_values last;

    /*
     * Values of the legendre polynomials of the last but one successful cache access.
     */
    struct cache_values lastlast;

    /*
     * Values of the legendre polynomials of the current access
     */
    struct cache_values current;

#ifdef CACHE_STATS
    /*
     * Number of successfull cache accesses
     */
    unsigned long cache_hits;

    /*
     * Number of unsuccessfull cache accesses.
     * This is slow, because the values of plm need to be calculated using
     * the recurrence relations.
     */
    unsigned long cache_misses;
#endif
};


/* ****************************** Prototypes ****************************** */

static inline struct cache_entry* get_cache_value(struct integ_context* context,
                                                  float80 x,
                                                  struct plm_cache* cache,
                                                  unsigned int index,
                                                  unsigned int iteration);

static inline void plm_recursive(struct integ_context* context,
                                 float80 x,
                                 struct cache_entry* current,
                                 struct cache_entry* last,
                                 struct cache_entry* lastlast);

static inline void calculate_cache_entry(struct integ_context* context,
                                         float80 x,
                                         struct cache_entry* entry);

static inline void plm_combination(struct integ_context* context,
                                   const float80 x,
                                   struct cache_entry* entry,
                                   plm_combination_t* comb);

/* ****************************** End Prototypes ****************************** */

/*
 * Calculate base^exp for integer values.
 * exp must be greater-equal zero
 */
static inline int pow_i(int base, int exp)
{
    int res = 1;
    while(exp > 0)
    {
        res *= base;
        exp--;
    }
    return res;
}


// TODO: Make this work for multiple threads
static struct plm_cache* glob_cache;


static void alloc_cache_values(struct cache_values* cv)
{
    size_t i;
    struct cache_iteration* ci;

    for(i = min_iteration; i <= max_iteration; ++i)
    {
        ci = &cv->iterations[i];
        /*
         * The uninitialized values at the moment are useless. So we won't set the
         * flag CACHE_FLAG_VALID.
         */
        ci->flags = 0U;
        /*
         * In each iteration i the romberg integration uses 2^(i-2) new points.
         * We will allocate the memory for it now.
         */
        ci->entry = xmalloc(pow_i(2, i - 2) * sizeof(struct cache_entry));
    }
}


static void free_cache_values(struct cache_values* cv)
{
    size_t i;
    struct cache_iteration* ci;

    for(i = min_iteration; i <= max_iteration; ++i)
    {
        ci = &cv->iterations[i];
        xfree(ci->entry);
    }
}


/*
 * Invalidate the cache values. We need this if we are in a new cycle or if we
 * start to calculate a new matsubara term.
 */
static inline void inval_cache_values(struct cache_values* cv)
{
    size_t i;
    struct cache_iteration* ci;

    for(i = min_iteration; i <= max_iteration; ++i)
    {
        ci = &cv->iterations[i];
        ci->flags &= ~CACHE_FLAG_VALID;
    }
}


/*
 * Shift the values of the plm_cache
 * last       ->     lastlast
 * current    ->     last
 * lastlast   ->     current
 * This is fast, because we don't need to copy the values in memory. We only need to change
 * some pointers.
 */
static inline void shift_cache_values(struct plm_cache* cache)
{
    struct cache_entry* entry;
    unsigned char flags;
    size_t i;

    for(i = min_iteration; i <= max_iteration; ++i)
    {
        entry = cache->lastlast.iterations[i].entry;
        flags = cache->lastlast.iterations[i].flags;
        
        cache->lastlast.iterations[i].entry = cache->last.iterations[i].entry;
        cache->lastlast.iterations[i].flags = cache->last.iterations[i].flags;
        
        cache->last.iterations[i].entry = cache->current.iterations[i].entry;
        cache->last.iterations[i].flags = cache->current.iterations[i].flags;
        
        cache->current.iterations[i].entry = entry;
        cache->current.iterations[i].flags = flags;
        cache->current.iterations[i].flags &= ~CACHE_FLAG_VALID;
    }
}


void plm_create_cache(casimir_t* casimir)
{
    // TODO: Make this work for multiple threads
    struct plm_cache* cache = glob_cache;
    
    cache = xmalloc(sizeof(struct plm_cache));
    // TODO: Make this work for multiple threads
    glob_cache = cache;

    cache->n = 0;
    cache->m = 0;
    cache->casimir = casimir;
    
#ifdef CACHE_STATS
    cache->cache_hits   = 0;
    cache->cache_misses = 0;
#endif

    /*
     * Allocate the memory for the last and the first cache access
     */
    alloc_cache_values(&cache->current);
    alloc_cache_values(&cache->last);
    alloc_cache_values(&cache->lastlast);

}


void plm_destroy_cache()
{
    // TODO: Make this work for multiple threads
    struct plm_cache* cache = glob_cache;

    free_cache_values(&cache->current);
    free_cache_values(&cache->last);
    free_cache_values(&cache->lastlast);

    xfree(cache);
}


void plm_cache_init(struct integ_context* context, int n)
{
    struct plm_cache* cache;

    // TODO: Make this work for multiple threads
    cache = glob_cache;

    /*
     * If l2 == m is true, we have the first integration in one cycle.
     * We need to set up the cache for the new l1pl2 etc.
     */
    if(context->l2 == context->m
       || context->m != cache->m
       || context->l2 + context->l1 != cache->l1pl2
       || cache->n != n)
    {
        cache->l1pl2 = context->l1 + context->l2;
        cache->m     = context->m;
        cache->n     = n;
                
        /*
         * The last values are useless. We need to invalidate them
         */
        inval_cache_values(&cache->last);
        inval_cache_values(&cache->lastlast);
        inval_cache_values(&cache->current);
    }
    else
    {
        /*
         * New integration with the same n and the same m and the same l1 + l2.
         * E.g. the last iteration was l1 = 7, l2 = 3 and the current iteration is l1 = 6, l2 = 4.
         * We can reuse the old values, but we need to shift them (because now, the values from
         * last_iteration need to be stored it the member "last").
         */
        shift_cache_values(cache);
    }
}


void plm_cache_free(struct integ_context* context)
{
#ifdef CACHE_STATS
    struct plm_cache* cache;
    unsigned long ratio;

    // TODO: Make this work for multiple threads
    cache = glob_cache;

    if(cache->cache_hits + cache->cache_misses == 0)
        ratio = 0UL;
    else
        ratio = 100UL * cache->cache_hits / (cache->cache_hits + cache->cache_misses);

    printf("Number:  %lu\nSuccess: %lu\nFailed:  %lu\nRatio:   %lu%%\n",
           cache->cache_hits + cache->cache_misses,
           cache->cache_hits,
           cache->cache_misses,
           ratio);
#endif
}


#ifdef DEBUG_PLM_CACHE
static void check_values(int l1, int l2, int m, float80 x, plm_combination_t* comb)
{
#define ALMOST_EQUAL(x,y) ((x) == (y) || fabs80(1 - (x)/(y)) < 1e-9)
    plm_combination_t real_values;

    plm_PlmPlm(l1, l2, m, x, &real_values);

    assert(ALMOST_EQUAL(real_values.lnPl1mPl2m, comb->lnPl1mPl2m));
    assert(ALMOST_EQUAL(real_values.lnPl1mdPl2m, comb->lnPl1mdPl2m));
    assert(ALMOST_EQUAL(real_values.lndPl1mPl2m, comb->lndPl1mPl2m));
    assert(ALMOST_EQUAL(real_values.lndPl1mdPl2m, comb->lndPl1mdPl2m));

    assert(real_values.sign_Pl1mPl2m == comb->sign_Pl1mPl2m);
    assert(real_values.sign_Pl1mdPl2m == comb->sign_Pl1mdPl2m);
    assert(real_values.sign_dPl1mPl2m == comb->sign_dPl1mPl2m);
    assert(real_values.sign_dPl1mdPl2m == comb->sign_dPl1mdPl2m);
}
#endif


void plm_cache_PlmPlm(struct integ_context* context, float80 x, plm_combination_t* comb,
                      unsigned int index, unsigned int iteration)
{
    int l1, l2, m;
    struct plm_cache* cache;
    struct cache_entry* entry = NULL;

    l1 = context->l1;
    l2 = context->l2;
    m  = context->m;
    
    // TODO: Make this work for multiple threads
    cache = glob_cache;

    /*
     * If we calculate a bigger iteration than "max_iteration" something has gone wrong.
     * We should choose "max_iteration" as big as in "integration_drude.c".
     */
    assert(iteration <= max_iteration);
    
    if(l1 + l2 < 2 * min_l)
    {
        /*
         * Don't use the cache for the small multipoles. It won't speed up the calculation
         * either.
         */
        plm_PlmPlm(l1, l2, m, x, comb);
        return;
    }

    if(iteration >= min_iteration)
    {
        entry = get_cache_value(context, x, cache, index, iteration);
        plm_combination(context, x, entry, comb);

#ifdef DEBUG_PLM_CACHE
        check_values(l1, l2, m, x, comb);
#endif
    }
    else
    {
        plm_PlmPlm(l1, l2, m, x, comb);
    }
}


static inline struct cache_entry* get_cache_value(struct integ_context* context,
                                                  float80 x,
                                                  struct plm_cache* cache,
                                                  unsigned int index,
                                                  unsigned int iteration)
{
    struct cache_iteration* last_i;
    struct cache_iteration* lastlast_i;
    struct cache_iteration* current_i;

    struct cache_entry* last;
    struct cache_entry* lastlast;
    struct cache_entry* current;
    
    last_i     = &cache->last.iterations[iteration];
    lastlast_i = &cache->lastlast.iterations[iteration];
    current_i  = &cache->current.iterations[iteration];

    if(!(last_i->flags & CACHE_FLAG_VALID
         && lastlast_i->flags & CACHE_FLAG_VALID))
    {
        /*
         * Can't use the last values. Need to calculate them.
         */
        calculate_cache_entry(context, x, &current_i->entry[index]);
        current  = &current_i->entry[index];

#ifdef CACHE_STATS
        /*
         * Update the statistics of this cache
         */
        cache->cache_misses++;
#endif
    }
    else
    {
        last     = &last_i->entry[index];
        lastlast = &lastlast_i->entry[index];
        current  = &current_i->entry[index];

        /*
         * Calculate the value from the last two tuples of l1l2.
         */
        current->l1 = context->l1;
        current->l2 = context->l2;
        plm_recursive(context, x, current, last, lastlast);

#ifdef CACHE_STATS
        /*
         * Update the statistics of this cache
         */
        cache->cache_hits++;
#endif
    }

    current->common_sign = MPOW(context->m % 2);

    /*
     * Since the integration function calculates all the points for one iteration,
     * we can set all points in this iteration to valid (regardless of the index).
     */
    current_i->flags |= CACHE_FLAG_VALID;
    
    return current;
}


/*
 * Calculate Legendre Polynomials and its derivatives.
 * They are stored in a "struct cache_entry".
 * See "plm_PlmPlm" in sfunc.c.
 */
static inline void calculate_cache_entry(struct integ_context* context,
                                         float80 x,
                                         struct cache_entry* entry)
{
    const int l1   = context->l1;
    const int l2   = context->l2;
    const int lmax = MAX(l1, l2) + 1;
    const int m    = context->m;
    float80 lnPlm[lmax-m+1];
    sign_t signs[lmax-m+1];

    entry->l1 = l1;
    entry->l2 = l2;

    plm_lnPlm_array(lmax, context->m, x, lnPlm, signs);

    entry->lnPl1     = lnPlm[l1 - m];
    entry->sign_Pl1  = signs[l1 - m];

    entry->lnPl2     = lnPlm[l2 - m];
    entry->sign_Pl2  = signs[l2 - m];

    entry->lnPl1p1    = lnPlm[l1 + 1 - m];
    entry->sign_Pl1p1 = signs[l1 + 1 - m];

    entry->lnPl2p1    = lnPlm[l2 + 1 - m];
    entry->sign_Pl2p1 = signs[l2 + 1 - m];
}


static inline void plm_recursive(struct integ_context* context,
                                 float80 x,
                                 struct cache_entry* current,
                                 struct cache_entry* last,
                                 struct cache_entry* lastlast)
{
    sign_t sign;
    const float80 logx = log80(x);
    const int l1       = current->l1;
    const int l2       = current->l2;
    const int m        = context->m;
    
    assert(last->l1 == current->l1 + 1);
    assert(last->l2 == current->l2 - 1);
    assert(lastlast->l1 == current->l1 + 2);
    assert(lastlast->l2 == current->l2 - 2);

    /*
     * current->Pl2 = ( (2*l2-1) * x * last->Pl2 - (l2 - 1 + m) * lastlast->Pl2 ) / (l2 - m)
     */
    current->lnPl2 = logadd_s( log80(2 * l2 - 1) + logx + last->lnPl2,
                               last->sign_Pl2,
                               log80(l2 - 1 + m) + lastlast->lnPl2,
                               -lastlast->sign_Pl2,
                               &sign );
    current->lnPl2 -= log80(l2 - m);
    current->sign_Pl2 = sign;

    /*
     * current->Pl1 = ( (2*l1+3) * x * last->Pl1 - (l1 + 2 - m) * lastlast->Pl1 ) / (l1 + 1 + m)
     */
    current->lnPl1 = logadd_s( log80(2 * l1 + 3) + logx + last->lnPl1,
                               last->sign_Pl1,
                               log80(l1 + 2 - m) + lastlast->lnPl1,
                               -lastlast->sign_Pl1,
                               &sign );
    current->lnPl1 -= log80(l1 + 1 + m);
    current->sign_Pl1 = sign;

    /*
     * current->Pl2p1 = ( (2*l2+1) * x * last->Pl2p1 - (l2 + m) * lastlast->Pl2p1 ) / (l2 + 1 - m)
     */
    current->lnPl2p1 = logadd_s( log80(2 * l2 + 1) + logx + last->lnPl2p1,
                               last->sign_Pl2p1,
                               log80(l2 + m) + lastlast->lnPl2p1,
                               -lastlast->sign_Pl2p1,
                               &sign );
    current->lnPl2p1 -= log80(l2 + 1 - m);
    current->sign_Pl2p1 = sign;

    /*
     * current->Pl1p1 = ( (2*l1+5) * x * last->Pl1p1 - (l1 + 3 - m) * lastlast->Pl1p1 ) / (l1 + 2 + m)
     */
    current->lnPl1p1 = logadd_s( log80(2 * l1 + 5) + logx + last->lnPl1p1,
                                 last->sign_Pl1p1,
                                 log80(l1 + 3 - m) + lastlast->lnPl1p1,
                                 -lastlast->sign_Pl1p1,
                                 &sign );
    current->lnPl1p1 -= log80(l1 + 2 + m);
    current->sign_Pl1p1 = sign;
}


static inline void plm_combination(struct integ_context* context,
                                   const float80 x,
                                   struct cache_entry* entry,
                                   plm_combination_t* comb)
{
    float80 lndPl1;
    float80 lndPl2;

    sign_t  sign_lndPl1;
    sign_t  sign_lndPl2;

    const int l1              = context->l1;
    const int l2              = context->l2;
    const int m               = context->m;
    const float80 logx2m1     = log80(pow_2(x) - 1);
    const float80 logx        = log80(x);

        
    lndPl1 = logadd_s( log80(l1-m+1) + entry->lnPl1p1,
                       entry->sign_Pl1p1,
                       log80(l1+1) + logx + entry->lnPl1,
                       -entry->sign_Pl1,
                       &sign_lndPl1) - logx2m1;

    lndPl2 = logadd_s( log80(l2-m+1) + entry->lnPl2p1,
                       entry->sign_Pl2p1,
                       log80(l2+1) + logx + entry->lnPl2,
                       -entry->sign_Pl2,
                       &sign_lndPl2) - logx2m1;
    
    /* Pl1m*Pl2m */
    comb->lnPl1mPl2m      = entry->lnPl1 + entry->lnPl2;
    comb->sign_Pl1mPl2m   = entry->common_sign * entry->sign_Pl1 * entry->sign_Pl2;

    /* Pl1m*dPl2m */
    comb->lnPl1mdPl2m     = entry->lnPl1 + lndPl2;
    comb->sign_Pl1mdPl2m  = entry->common_sign * entry->sign_Pl1 * sign_lndPl2;

    /* dPl1m*Pl2m */    
    comb->lndPl1mPl2m     = lndPl1 + entry->lnPl2;
    comb->sign_dPl1mPl2m  = entry->common_sign * sign_lndPl1 * entry->sign_Pl2;

    /* dPl1m*dPl2m */
    comb->lndPl1mdPl2m    = lndPl1 + lndPl2;
    comb->sign_dPl1mdPl2m = entry->common_sign * sign_lndPl1 * sign_lndPl2;
}