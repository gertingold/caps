#ifndef __SFUNC_H
#define __SFUNC_H

#include <stdlib.h>
#include <math.h>

#include "floattypes.h"
#include "libcasimir.h"

#define pow_2(x) ((x)*(x))
#define pow_3(x) ((x)*(x)*(x))
#define pow_4(x) ((x)*(x)*(x)*(x))
#define pow_5(x) ((x)*(x)*(x)*(x)*(x))
#define pow_6(x) ((x)*(x)*(x)*(x)*(x)*(x))
#define pow_7(x) ((x)*(x)*(x)*(x)*(x)*(x)*(x))

/* calculate pow(-1,a) = -1**a */
#define MPOW(a) (1-2*((signed char)(a) & 1))

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((((a))>((b)))?((a)):((b)))
#endif

typedef struct {
    float80 lnPl1mPl2m;
    int sign_Pl1mPl2m;

    float80 lndPl1mPl2m;
    int sign_dPl1mPl2m;

    float80 lnPl1mdPl2m;
    int sign_Pl1mdPl2m;

    float80 lndPl1mdPl2m;
    int sign_dPl1mdPl2m;
} plm_combination_t;

inline float80 logadd(const float80 a, const float80 b);
inline float80 logadd_s(const float80 a, const sign_t sign_a, const float80 b, const sign_t sign_b, sign_t *sign);
inline float80 logadd_m(const float80 list[], const int len);
inline float80 logadd_ms(const float80 list[], const sign_t signs[], const int len, sign_t *sign);

float80 bessel_lnInu(const int n, const float80 x);
float80 bessel_lnKnu(const int n, const float80 x);
void bessel_lnInuKnu(int nu, const float80 x, float80 *lnInu_p, float80 *lnKnu_p);

float80 ln_doublefact(int n);

inline void plm_lnPlm_array(int lmax, int m, float80 x, float80 lnplm[], sign_t sign[]);
float80 plm_lnPlm (int l, int m, float80 x, sign_t *sign);
float80 plm_Plm   (int l, int m, float80 x);
float80 plm_lndPlm(int l, int m, float80 x, sign_t *sign);
float80 plm_dPlm  (int l, int m, float80 x);

void plm_PlmPlm(int l1, int l2, int m, float80 x, plm_combination_t *res);

inline float80 gaunt_log_a0(int n, int nu, int m);
inline float80 gaunt_a0(int n,int nu,int m);
void gaunt(const int n, const int nu, const int m, float80 a_tilde[]);

inline int gaunt_qmax(const int n, const int nu, const int m);

#endif
