#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "material.h"
#include "utils.h"
#include "sfunc.h"

/* Parse a string in the form of:
 *      key separator value
 * where value is a double. If key or separator is not found, false is
 * returned. If the string is matched successfully, value is set.
 */
static bool _parse(const char *line, const char *key, const char separator, double *value)
{
    char *p = strstr(line, key);
    if(p != NULL)
    {
        p = strchr(p+strlen(key), separator); /* find separator */
        if(p != NULL)
        {
            *value = atof(p+1);
            return true;
        }
    }

    return false;
}

/** @brief Initialize material
 *
 * The material properties are read from the file given by filename.
 *
 * This function temporarily overwrites the value of LC_NUMERIC in the
 * environment and restores. The value is restored before returning from this
 * function.
 *
 * @param [in] filename
 * @retval material
 */
material_t *material_init(const char *filename)
{
    /* prototype for setenv to avoid gcc warning */
    int setenv(const char *name, const char *value, int overwrite);

    char backup_lc_numeric[512] = { 0 };
    size_t points = 0;
    size_t size = 128; /* max number of elems of array */
    char line[2048];
    FILE *f = fopen(filename, "r");

    if(f == NULL)
        return NULL;

    material_t *material = xmalloc(sizeof(material_t));

    memset(material->filename, '\0', sizeof(material->filename));
    strncpy(material->filename, filename, sizeof(material->filename)/sizeof(char)-sizeof(char));

    /* initialize to 0 */
    material->omegap_low  = 0;
    material->gamma_low   = 0;
    material->omegap_high = 0;
    material->gamma_high  = 0;

    material->xi    = xmalloc(size*sizeof(double));
    material->epsm1 = xmalloc(size*sizeof(double));

    /* copy environment value of LC_NUMERIC */
    {
        const char *p = getenv("LC_NUMERIC");
        if(p != NULL)
        {
            strncpy(backup_lc_numeric, p, sizeof(backup_lc_numeric)-sizeof(char));
            setenv("LC_NUMERIC", "C", 1);
        }
    }

    while(fgets(line, sizeof(line)/sizeof(char), f) != NULL)
    {
        if(line[0] == '#')
        {
            _parse(line, "omegap_low",  '=', &material->omegap_low);
            _parse(line, "gamma_low",   '=', &material->gamma_low);
            _parse(line, "omegap_high", '=', &material->omegap_high);
            _parse(line, "gamma_high",  '=', &material->gamma_high);

            continue;
        }

        const char *p = strchr(line, ' ');
        if(p != NULL)
        {
            double xi = atof(line), epsm1 = atof(p)-1;

            /* increase buffer */
            if(points >= size)
            {
                size *= 2;
                material->xi    = xrealloc(material->xi,    size*sizeof(double));
                material->epsm1 = xrealloc(material->epsm1, size*sizeof(double));
            }

            material->xi[points]    = xi;
            material->epsm1[points] = epsm1;

            /* check if the values of xi are sorted (ascending) */
            if(points > 0 && xi <= material->xi[points-1])
            {
                material_free(material);
                material = NULL;
                goto out;
            }

            points++;
        }
    }

    material->xi_min = material->xi[0];
    material->xi_max = material->xi[points-1];
    material->points = points;

out:
    /* restore environment value of LC_NUMERIC */
    if(strlen(backup_lc_numeric))
        setenv("LC_NUMERIC", backup_lc_numeric, 1);

    if(f != NULL)
        fclose(f);

    return material;
}

void material_free(material_t *material)
{
    if(material != NULL)
    {
        xfree(material->xi);
        xfree(material->epsm1);
        xfree(material);
    }
}

void material_info(material_t *material, FILE *stream, const char *prefix)
{
    if(prefix == NULL)
        prefix = "";

    fprintf(stream, "%sfilename    = %s\n",  prefix, material->filename);
    fprintf(stream, "%spoints      = %zu\n", prefix, material->points);
    fprintf(stream, "%sxi_min      = %g\n",  prefix, material->xi_min);
    fprintf(stream, "%sxi_max      = %g\n",  prefix, material->xi_max);
    fprintf(stream, "%somegap_high = %g\n",  prefix, material->omegap_high);
    fprintf(stream, "%sgamma_high  = %g\n",  prefix, material->gamma_high);
    fprintf(stream, "%sgamma_low   = %g\n",  prefix, material->gamma_low);
    fprintf(stream, "%somegap_low  = %g\n",  prefix, material->omegap_low);
}

double material_epsilonm1(double xi, void *args)
{
    material_t *material = (material_t *)args;
    const double xi_min = material->xi_min, xi_max = material->xi_max;

    if(xi < xi_min)
    {
        double omegap = material->omegap_low, gamma_ = material->gamma_low;
        return pow_2(omegap)/(xi*(xi+gamma_));
    }
    else if(xi > xi_max)
    {
        double omegap = material->omegap_high, gamma_ = material->gamma_high;
        return pow_2(omegap)/(xi*(xi+gamma_));
    }

    /* do binary search */
    int left = 0, right = material->points-1;

    /* find indices of right and left value */
    while(right-left != 1)
    {
        int middle = (right+left)/2;
        double xi_middle = material->xi[middle];
        if(xi_middle > xi)
            right = middle;
        else
            left = middle;
    }

    const double xi_lower = material->xi[left], xi_upper = material->xi[right];
    const double epsm1_lower = material->epsm1[left], epsm1_upper = material->epsm1[right];

    /* linear interpolation */
    return epsm1_lower + (xi-xi_lower)*(epsm1_upper-epsm1_lower)/(xi_upper-xi_lower);
}