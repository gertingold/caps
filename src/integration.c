/**
 * @file   integration.c
 * @author Michael Hartmann <caps@speicherleck.de>
 * @date   December, 2018
 * @brief  Perform integration for arbitrary materials
 */

#include <math.h>
#include <stdint.h>
#include <inttypes.h>

#include "quadpack.h"

#include "constants.h"
#include "bessel.h"
#include "plm.h"
#include "utils.h"
#include "misc.h"
#include "libcaps.h"
#include "logfac.h"
#include "integration.h"

/** arguments for integrand in function K_integrand */
typedef struct
{
    int nu,m;
    polarization_t p; /* TE or TM */
    double factor,alpha,log_normalization;
    caps_t *caps;
} integrand_t;

/* Create a hash from l1, l2 and p. The hash function breaks if l1 or l2 >
 * 2^31. This, however, exceeds the resources available by orders of
 * magnitude. Other things will break earlier...
 */
static uint64_t hash(uint64_t l1, uint64_t l2, uint64_t p)
{
    return (l1 << 32) | (l2 << 1) | p;
}

/* this is a function only used in K_estimate */
static double _f(double x, int nu, int m, double alpha)
{
    TERMINATE(x <= 1, "x=%g, nu%d, m=%d, alpha=%g", x, nu, m, alpha);

    if(x == 1)
    {
        if(m != 1)
            return -INFINITY;

        return alpha - logi(nu+2)-logi(nu+1)-logi(nu)-logi(nu-1)+log(8);
    }

    if(m == 0)
        return alpha*x-lnPlm(nu,2,x);
    else
        return alpha*x-lnPlm(nu,2*m,x)+log(x*x-1);
}

/** @brief Estimate position and width of peak
 *
 * We want to estimate the position and the width of the peak of
 * the integrand for \f$m>0\f$
 * \f[
 *  \int_1^\infty \mathrm{d}x \, r_p \frac{e^{-\alpha x}}{x^2-1} P_\nu^{2m}(x) = \int_1^\infty \mathrm{d}x \, r_p g(x) = \int_1^\infty \mathrm{d}x \, r_p e^{-f(x)}
 * \f]
 * and for \f$m=0\f$
 * \f[
 *  \int_1^\infty \mathrm{d}x \, r_p e^{-\alpha x} P_\nu^2(x) = \int_1^\infty \mathrm{d}x \, r_p g(x) = \int_1^\infty \mathrm{d}x \, r_p e^{-f(x)}
 * \f]
 * with (\f$m>0\f$)
 * \f[
 *  f(x) = \alpha x - \log P_\nu^{2m}(x) + \log(x^2-1),
 * \f]
 * and (\f$m=0\f$)
 * \f[
 *  f(x) = \alpha x - \log P_\nu^2(x)\,.
 * \f]
 * We will assume that the Fresnel coefficient \f$r_p\f$ varies slowly with
 * respect to the width of the peak and set it to 1.
 *
 * We find the maximum of \f$f(x)\f$ using Newton's method on \f$f'(x)\f$. With
 * the maximum \f$x_\mathrm{max} \f$ and the second derivative at the maximum
 * \f$f''(x_\mathrm{max})\f$, we estimate the width of the peak and the value of
 * the integral using Laplace's method:
 * \f[
 *  \int_1^\infty \mathrm{d}x \, e^{-f(x)} \approx \sqrt{\frac{2\pi}{-f''(x_\mathrm{max})}} e^{-f(x_\mathrm{max})}
 * \f]
 *
 * The left border a and the right border b are determined by eps, such that
 * \f[
 *  e^{-f(a)} \approx e^{-f(b)} \approx \epsilon e^{-f(x_\mathrm{max})} \,.
 * \f]
 * However, a cannot be smaller than 1.
 *
 * @param [in] nu parameter \f$\nu\f$
 * @param [in] m parameter \f$m\f$
 * @param [in] alpha \f$\alpha\f$
 * @param [in] eps \f$\epsilon\f$
 * @param [out] a left border
 * @param [out] b right border
 * @param [out] approx logarithm of estimated value of integral
 */
double K_estimate(int nu, int m, double alpha, double eps, double *a, double *b, double *approx)
{
    const int maxiter = 75;
    const int mpos = (m > 0) ? 1 : 0;
    const int m_ = MAX(m,1);
    double fxmax, fp, fpp, xmax;

    #define f(x) _f((x), nu, m, alpha)

    if(m == 1)
    {
        /* g(x)  = Plm(nu,2,x)*exp(-αx)/(x²-1) = exp(-αx) P_nu''(x)
         * g'(x) = exp(-αx) [ -αP_nu''(x) + P_nu'''(x) ]
         * P_n'(1)   = n(n+1)/2
         * P_n''(1)  = (n-1)n(n+1)(n+2)/8 (triangular numbers)
         * P_n'''(1) = (n-2)(n-1)n(n+1)(n+2)(n+3)/48 (OEIS A240440)
         *
         * if m = 1 and (n-2)(n+3)/6 < α  =>  there is no maximum
         *
         * The derivative is given by:
         * g'(1) = exp(-α) (n-1)n(n+1)(n+2)/8 [ (n-2)(n+3)/6 -α ]
         *
         * If the derivative is smaller than some threshold, we assume that the
         * maximum is at x=1.
         */

        /* don't remove the dots in order to avoid integer overflows */
        const double threshold = (nu-2.)*(nu+3.)/(6*alpha);

        if(threshold < 2)
        {
            *a = 1;
            *b = 1-log(eps)/alpha;

            const double logt1 = 1.5*log(alpha)+log(2/M_PI)/2+bessel_logKn_half(nu,alpha);
            const double logt2 = -alpha+log(alpha+nu*(nu+1)/2.);
            const double arg = -exp(logt2-logt1);

            fxmax = alpha-log( (nu-1.)*nu*(nu+1.)*(nu+2)/8. );
            xmax = 1;

            /* for m=2 and PR we can evaluate the integral analytically. However,
             * if alpha is very large, then arg may become -∞. In this case, we
             * just use the maximum of the integrand as an approximation. This is
             * ok, because we use approx only to scale the integrand.
             */
            if(fabs(arg) < 1)
                /* exact evaluation */
                *approx = logt1 + log1p(arg);
            else
                /* use maximum as an estimate for integral */
                *approx = -fxmax;

            goto bordercheck;
        }
    }

    /* initial guess of maximum */
    if(nu == (2*m))
    {
        int l = nu/2;
        double ratio = (l-1)/alpha;
        xmax = ratio + sqrt(1+pow_2(ratio));
    }
    else
        xmax = sqrt(1+pow_2((nu+0.5)/alpha));

    /* find position of peak: we use Newton's method to find the root of f'(x) */
    int i;
    for(i = 0; i < maxiter; i++)
    {
        const double xold = xmax, x2m1 = xmax*xmax-1;
        double d, d2;
        d = dlnPlm(nu, 2*m_, xmax, &d2);

        fp  = alpha -d  +mpos*2*xmax/x2m1;
        fpp =       -d2 -mpos*2*(xmax*xmax+1)/pow_2(x2m1);

        xmax = xmax-fp/fpp;

        if(xmax <= 1)
            xmax = 1+(xold-1)/2;

        /* if xmax is close to 1, compute maximum with higher precision */
        const double delta = fabs(xmax-xold);
        if(delta < 1e-13 || (xmax > 1.001 && delta < 1e-6))
            break;
    }

    TERMINATE(isnan(xmax) || isinf(xmax), "xmax=%g, nu=%d, m=%d, alpha=%.15g", xmax, nu, m, alpha);

    fxmax = f(xmax);

    TERMINATE(isnan(fxmax) || isinf(fxmax), "xmax=%.15g, fxmax=%g, nu=%d, m=%d, alpha=%.15g", xmax, fxmax, nu, m, alpha);

    TERMINATE(isnan(fpp) || isinf(fpp) || fpp < 0, "xmax=%.15g, fxmax=%.15g, fpp=%.15g, nu=%d, m=%d, alpha=%.15g", xmax, fxmax, fpp, nu, m, alpha);

    {
        /* approximation using Laplace's method */
        *approx = log(2*M_PI/fpp)/2-fxmax;

        /* estimate width, left and right borders */
        const double width = -log(eps)/sqrt(fpp);

        *a = fmax(1, xmax-width);
        *b = xmax+width;
    }

    /*
    printf("xmax=%.10g\n", xmax);
    printf("fxmax=%.10g\n", fxmax);
    printf("fxmax=%.10g\n", f(xmax));
    printf("fpp=%.10g\n", fpp);
    */

bordercheck:

    /* check left border */
    if(*a > 1)
    {
        for(i = 0; i < maxiter; i++)
        {
            const double fa = f(*a);
            if(exp(fxmax-fa) < eps)
                break;

            *a = 1+0.5*(*a-1);
        }

        TERMINATE(i == maxiter, "nu=%d, m=%d, alpha=%g, xmax=%g, f(xmax)=%g, a=%g", nu, m, alpha, xmax, fxmax, *a);
    }

    /* check right border */
    for(i = 0; i < maxiter; i++)
    {
        const double fb = f(*b);
        if(exp(fxmax-fb) < eps)
            break;

        *b = 1+2*(*b-1);
    }

    TERMINATE(i == maxiter, "nu=%d, m=%d, alpha=%g, xmax=%g, f(xmax)=%g, b=%g", nu, m, alpha, xmax, fxmax, *b);

    #if 0 /* neat for debugging */
    printf("estimate: a=%g, b=%g, I=%.15g, xmax=%g\n", *a, *b, *approx, xmax);
    #endif

    return xmax;

    #undef f
}

static double K_integrand(double x, void *args_)
{
    double v, rTE, rTM;
    integrand_t *args = (integrand_t *)args_;
    caps_t *caps = args->caps;

    x *= args->factor;

    const int nu = args->nu, m = args->m;
    const double log_normalization = args->log_normalization;
    const double alpha = args->alpha;
    const double xi_tilde = alpha/2;
    const double x2m1 = (x+1)*(x-1);

    if(m)
        v = exp(-log_normalization + lnPlm(nu,2*m,x)-alpha*x-log(x2m1));
    else
        v = exp(-log_normalization + lnPlm(nu,2,x)-alpha*x);

    /* compute Fresnel coefficients rTE, rTM */
    caps_fresnel(caps, xi_tilde, xi_tilde*sqrt(x2m1), &rTE, &rTM);

    TERMINATE(isnan(v) || isinf(v), "x=%g, nu=%d, m=%d, alpha=%g, v=%g, log_normalization=%g, lnPlm=%g | %g", x, nu, m, alpha, v, log_normalization, lnPlm(nu,2*m,x),
    -log_normalization + lnPlm(nu,2*m,x)-alpha*x);

    if(args->p == TE)
        return rTE*v;
    else
        return rTM*v;
}

static double _caps_integrate_K(integration_t *self, int nu, polarization_t p, sign_t *sign)
{
    double xmax,log_normalization,a,b;
    const int m = self->m;
    const double eps = 1e-6;
    const double alpha = self->alpha;
    const double epsrel = self->epsrel;

    /* some analytical solutions for perfect reflectors */
    #if 0
    if(self->is_pr)
    {
        if(p == TE)
            *sign = -1;
        else
            *sign = +1;

        if(m == 0)
        {
            /* use analytical result for m=0 and PR
             * integrand: r_p exp(-α*x)*P_ν^2(x)
             * integral: 2*exp(-α) + sqrt(2/(pi*α)) [ (ν+1)(ν+2)K_{ν+1/2}(α) - 2α K_(ν+3/2)(α) ]
             *           \- t0 --/   \----- C ----/   \------- t1 ---------/   \---- t2 -----/
             */
            const double logC = log(2/(alpha*M_PI))/2;

            log_t terms[3];

            /* t1 */
            terms[0].v = log(2)-alpha;
            terms[0].s = +1;

            /* t2 */
            terms[1].v = logC+logi(nu+1)+logi(nu+2)+bessel_logKnu_half(nu,alpha);
            terms[1].s = +1;

            /* t3 */
            terms[2].v = logC+log(2)+log(alpha)+bessel_logKnu_half(nu+1,alpha);
            terms[2].s = -1;

            sign_t dummy;
            return logadd_ms(terms, 3, &dummy);
        }
        else if(nu == (2*m))
            return -0.5*log(M_PI)+(m-0.5)*log(2/alpha)+lfac(m-1)+lfac2(4*m-1)+bessel_logKnu_half(m-1,alpha);
        else if(nu == (2*m+1))
            return -0.5*log(M_PI)+(m-0.5)*log(2/alpha)+lfac(m-1)+lfac2(4*m-1)+bessel_logKnu_half(m,alpha)+logi(4*m+1);
        else if(m == 1)
        {
            /* use analytical result for m=1 and PR
             * integrand: r_p exp(-α*x)*P_ν^2(x)/(x²-1)
             * integral: α^(3/2)*sqrt(2/pi) K_(ν+½)(α) - exp(-α) [α + ν(ν+1)/2]
             *           \---------- t1 -------------/   \-------- t2 --------/
             */
            const double mu = 4*pow_2(nu+0.5);
            const double ae1 =     (mu-1) /(  8*alpha);
            const double ae2 = ae1*(mu-9) /(2*8*alpha);
            const double ae3 = ae2*(mu-25)/(3*8*alpha);

            /* Use asymptotic expansion
             * K_{ν+½}(α) = sqrt(π/(2α)) e^{-α} ( 1 + ae1 + ae2 + ae3 + ... )
             * to avoid loss of significance. The error is not larger than the
             * first neglected term.
             */
            if(ae3 < 1e-8)
                /* integral: α*exp(-α)*( ae1 + ae2 + ae3 + ... -ν(ν+1)/(2α)) */
                return log(alpha)-alpha+log(ae1+ae2+ae3-nu*(nu+1.)/(2*alpha));

            const double logt1 = 1.5*log(alpha)+log(2/M_PI)/2+bessel_logKnu_half(nu,alpha);
            const double logt2 = -alpha+log(alpha+nu*(nu+1.)/2.);

            return logt1 + log1p(-exp(logt2-logt1));
        }
    }
    #endif

    integrand_t args = {
        .nu   = nu,
        .m    = m,
        .p    = p,
        .alpha  = alpha,
        .factor = 1,
        .caps = self->caps
    };

    xmax = K_estimate(nu, m, alpha, eps, &a, &b, &log_normalization);

    if(a < 1.0001)
        a = 1;
    //printf("nu=%d, m=%d, alpha=%g, a=%g, b=%g, xmax=%g\n", nu,m,alpha,a,b,xmax);

    /* log_normalization is the logarithm of the estimated maximum of the
     * integrand K
     */
    args.log_normalization = log_normalization;

    /* perform integrations in intervals [0,a], [a,b] and [b,∞] */
    int neval1 = 0, neval2 = 0, neval3 = 0, ier1 = 0, ier2 = 0, ier3 = 0;
    double abserr1 = 0, abserr2 = 0, abserr3 = 0, I1 = 0, I2 = 0, I3 = 0;

    /* I2: [a,b] */
    I2 = dqags(K_integrand, a, b, 0, epsrel, &abserr2, &neval2, &ier2, &args);

    /* I1: [1,a] */
    if(a > 1)
    {
        /* check if the contribution is so small that we can neglect the
         * contribution. The integrand is monotonically increasing in [1,a] and
         * the maximum is at x=a. So, a very simple estimate of the integral is
         * (a-1)*integrand(a)
         */
        const double fa = K_integrand(a, &args);
        if((a-1)*fa > I2*epsrel)
        {
            /* The contribution of this integral should be small, so use
             * Gauss-Kronrod G_K 7-15 as integration rule.
             */
            int limit = 200;
            I1 = dqage(K_integrand, 1, a, abserr2, 0, GK_7_15, &abserr1, &neval1, &ier1, &limit, &args);
        }
    }

    /* I3: [b,∞]
     * Make a substitution for the integrand, i.e., we don't integrate over z
     * but over t=z*α. The integrand exponentially decays as exp(-z*α), so
     * after the substitution it decays as exp(-t). This makes life easier for
     * the quadrature routine.
     */
    args.factor = 1/alpha;
    I3 = dqagi(K_integrand, b*alpha, 1, abserr2*alpha, epsrel, &abserr3, &neval3, &ier3, &args)/alpha;

    const double sum = I1+I2+I3;

    bool warn = ier1 != 0 || ier2 != 0 || ier3 != 0 || isnan(sum) || sum == 0;
    WARN(warn, "ier1=%d, ier2=%d, ier3=%d, nu=%d, m=%d, alpha=%.20g, xmax=%g, a=%g, b=%g, I1=%g, I2=%g, I3=%g", ier1, ier2, ier3, nu,m,alpha,xmax,a,b, I1, I2, I3);
    TERMINATE((*sign == 1 && p != TM) || (*sign==-1 && p != TE), "nu=%d, p=%d, sign=%d, sum=%g", nu, p, *sign, sum);

    *sign = SGN(sum);
    return log(fabs(sum)) + log_normalization;
}


/** @brief Compute integral \f$\mathcal{K}_{\nu,p}^{(m)}(\alpha)\f$
 *
 * This function solves for \f$m>0\f$ the integral
 * \f[
 *   \mathcal{K}_{\nu,p}^{(m)}(\alpha) = \int_1^\infty \mathrm{d}x \, r_p \frac{e^{-\alpha x}}{x^2-1} P_\nu^{2m}(x)
 * \f]
 * and for \f$m=0\f$ the integral
 * \f[
 *   \mathcal{K}_{\nu,p}^{(0)}(\alpha) = \int_1^\infty \mathrm{d}x \, r_p e^{-\alpha x} P_\nu^{2}(x) \,.
 * \f]
 *
 * The function returns the logarithm of the value of the integral and its sign.
 *
 * The projection of the wavevector onto the \f$xy\f$-plane is given by
 * \f$k=\frac{\xi}{c}\sqrt{x^2-1}\f$ and \f$\alpha=2\xi\mathcal{L}/c\f$.
 *
 * @param [in] self integration object
 * @param [in] nu parameter
 * @param [in] p polarization, either TE or TM
 * @param [out] sign sign of \f$\mathcal{K}_{\nu,p}^{(m)}(\alpha)\f$
 * @retval logK \f$\log\left|\mathcal{K}_{\nu,p}^{(m)}(\alpha)\right|\f$
 */
double caps_integrate_K(integration_t *self, int nu, polarization_t p, sign_t *sign)
{
    const size_t index = nu-2*self->m;

    /* extend cache if neccessary */
    if(index >= self->elems_cache_K)
    {
        const size_t oldsize = self->elems_cache_K;
        /* if cache is not sufficient, set the new size to double the amount we
         * need right now */
        self->elems_cache_K = 2*index;

        self->cache_K[0] = xrealloc(self->cache_K[0], self->elems_cache_K*sizeof(double));
        self->cache_K[1] = xrealloc(self->cache_K[1], self->elems_cache_K*sizeof(double));

        for(size_t i = oldsize; i < self->elems_cache_K; i++)
        {
            self->cache_K[0][i] = NAN;
            self->cache_K[1][i] = NAN;
        }
    }

    double K = self->cache_K[p][index];

    if(p == TM)
        *sign = 1;
    else
        *sign = -1;

    if(isnan(K))
    {
        /* compute and save integral */
        K = _caps_integrate_K(self, nu, p, sign);
        self->cache_K[p][index] = K;
    }

    return K;
}

/* eq. (3) */
static double _alpha(double p, double n, double nu)
{
    return (((pow_2(p)-pow_2(n+nu+1))*(pow_2(p)-pow_2(n-nu)))/(4*pow_2(p)-1));
}

static double _caps_integrate_I(integration_t *self, int l1, int l2, polarization_t p_, sign_t *sign)
{
    sign_t s;

    const int m_ = self->m > 0 ? self->m : 1;
    const double n  = l1, nu = l2, m = m_;
    const double n4 = l1+l2-2*m_;
    const int l1pl2 = l1+l2;

    /* eq. (24) */
    const int qmax = MIN(MIN(n,nu), (l1pl2-2*m_)/2);

    TERMINATE(qmax < 0, "l1=%d, l2=%d, m=%d\n", l1, l2, self->m);

    /* eq. (28) */
    const double Ap = -2*m*(n-nu)*(n+nu+1);

    /* eq. (20) */
    const double log_a0 = lfac(2*l1)-lfac(l1)+lfac(2*l2)-lfac(l2)+lfac(l1+l2)-lfac(2*l1pl2)+lfac(l1pl2-2*m_)-lfac(l1-m_)-lfac(l2-m_);

    double aq[qmax+1];
    log_t array[qmax+1];

    /* q = 0 */
    int q = 0;
    aq[q] = 1;
    double K = caps_integrate_K(self, l1pl2-2*q, p_, &s);
    array[q].s = s;
    array[q].v = K;

    if(qmax == 0)
        goto done;

    /* q = 1 */
    q = 1;
    /* eq. (29) */
    aq[q] = (n+nu-1.5)*(1-(2*n+2*nu-1)/(n4*(n4-1))*((m-n)*(m-n+1)/(2*n-1)+(m-nu)*(m-nu+1)/(2*nu-1)));
    K = caps_integrate_K(self, l1pl2-2*q, p_, &s);
    array[q].s = SGN(aq[q])*s;
    array[q].v = K+log(fabs(aq[q]));
    if(qmax == 1)
        goto done;

    q = 2;
    /* eq. (35) */
    aq[q] = (2*n+2*nu-1)*(2*n+2*nu-7)/4*( (2*n+2*nu-3)/(n4*(n4-1)) * ( (2*n+2*nu-5)/(2*(n4-2)*(n4-3)) \
                * ( (m-n)*(m-n+1)*(m-n+2)*(m-n+3)/(2*n-1)/(2*n-3) \
                + 2*(m-n)*(m-n+1)*(m-nu)*(m-nu+1)/((2*n-1)*(2*nu-1)) \
                + (m-nu)*(m-nu+1)*(m-nu+2)*(m-nu+3)/(2*nu-1)/(2*nu-3) ) - (m-n)*(m-n+1)/(2*n-1) \
                - (m-nu)*(m-nu+1)/(2*nu-1) ) +0.5);

    K = caps_integrate_K(self, l1pl2-2*q, p_, &s);
    array[q].s = SGN(aq[q])*s;
    array[q].v = K+log(fabs(aq[q]));

    if(qmax == 2)
        goto done;

    double log_scaling = 0;
    int count = 0;
    for(q = 3; q <= qmax; q++)
    {
        const double p = n+nu-2*q, p1 = p-2*m, p2 = p+2*m;

        if(Ap != 0)
        {
            /* eqs. (26), (27) */
            double c0 = (p+2)*(p+3)*(p1+1)*(p1+2)*Ap*_alpha(p+1,n,nu);
            double c1 = Ap*(Ap*Ap \
               + (p+1)*(p+3)*(p1+2)*(p2+2)*_alpha(p+2,n,nu) \
               + (p+2)*(p+4)*(p1+3)*(p2+3)*_alpha(p+3,n,nu));
            double c2 = -(p+2)*(p+3)*(p2+3)*(p2+4)*Ap*_alpha(p+4,n,nu);

            aq[q] = (c1*aq[q-1] + c2*aq[q-2])/c0;
        }
        else
            /* eq. (30) */
            aq[q] = (p+1)*(p2+2)*_alpha(p+2,n,nu)*aq[q-1] / ((p+2)*(p1+1)*_alpha(p+1,n,nu));

        if(fabs(aq[q]) > 1e100)
        {
            log_scaling += log(fabs(aq[q]));
            aq[q-1] /= fabs(aq[q]);
            aq[q] = SGN(aq[q]);
        }
        else if(fabs(aq[q]) < 1e-100 && fabs(aq[q]) > 0)
        {
            log_scaling -= log(fabs(aq[q]));
            aq[q-1] *= fabs(aq[q]);
            aq[q] = SGN(aq[q]);
        }

        K = caps_integrate_K(self, l1pl2-2*q, p_, &s);
        array[q].s = SGN(aq[q])*s;
        array[q].v = log_scaling+K+log(fabs(aq[q]));

        if((array[q].v - array[0].v) < -60)
        {
            if(++count >= 3)
                break;
        }
        else
            count = 0;
    }

    double log_I;
    done:
    log_I = log_a0+logadd_ms(array, MIN(q,qmax)+1, sign);
    TERMINATE(!isfinite(log_I), "l1=%d, l2=%d, m=%d, p=%d, alpha=%g, log_I=%g", l1, l2, self->m, p_, self->alpha, log_I);
    //TERMINATE((*sign == 1 && p_ != TM) || (*sign==-1 && p_ != TE), "l1=%d, l2=%d, p=%d, sign=%d, log_I=%g, q=%d", l1, l2, p_, *sign, log_I, q);

    return log_I;
}

/** @brief Compute integral \f$\mathcal{I}_{\ell_1,\ell_2,p}^{(m)}(\alpha)\f$
 *
 * Compute the integral
 * \f[
 * \mathcal{I}_{\ell_1,\ell_2,p}^{(m)}(\alpha) = \int_0^\infty \mathrm{d}x \, r_p \frac{e^{-\alpha x}}{x^2-1} P_{\ell_1}^m(x) P_{\ell_2}^m(x)
 * \f]
 *
 * This function returns the sign of the integral and its logarithmic value.
 *
 * @param [in] self integration object
 * @param [in] l1 parameter
 * @param [in] l2 parameter
 * @param [in] p polarization; either TE or TM
 * @param [out] sign sign of integral \f$\mathrm{sgn}\left(\mathcal{I}_{\ell_1,\ell_2,p}^{(m)}(\alpha)\right)\f$
 * @retval logI \f$\log\left| \mathcal{I}_{\ell_1,\ell_2,p}^{(m)}(\alpha) \right|\f$
 */
double caps_integrate_I(integration_t *self, int l1, int l2, polarization_t p, sign_t *sign)
{
    const int m = self->m;

    if(l1 < m || l2 < m)
    {
        *sign = 0;
        return -INFINITY;
    }

    /* simplification for perfect conductors */
    if(self->is_pr && p == TE)
    {
        const double v = caps_integrate_I(self, l1, l2, TM, sign);
        *sign = -1;
        return v;
    }

    /* I(l1,l2) = I(l2,l1)
     * Make sure that l1 >= l2
     */
    if(l1 < l2)
    {
        int temp = l1;
        l1 = l2;
        l2 = temp;
    }

    if(p == TM)
        *sign = 1;
    else
        *sign = -1;

    const uint64_t key = hash(l1,l2,p);
    double I = cache_lookup(self->cache_I, key);

    if(isnan(I))
    {
        /* compute and save integral */
        I = _caps_integrate_I(self, l1, l2, p, sign);
        cache_insert(self->cache_I, key, I);
    }

    return I;
}


/** @brief Initialize integration
 *
 * The aspect ratio L/R and the dielectric function of the metals
 * \f$\epsilon(i\xi)\f$ are taken from the caps object. The integration is
 * performed to a relative accuracy of epsrel.
 *
 * This function returns an object in order to compute the actual integrals.
 * The memory of this object has to be freed after use by a call to \ref
 * caps_integrate_free.
 *
 * The computation is sped up using caches. The number of elements of the cache
 * for the K integrals are proportional to ldim, the elements for the I
 * integrals are fixed. This value can be changed using the environmental
 * variable CAPS_CACHE_ELEMS.
 *
 * @param [in] caps CaPS object
 * @param [in] xi_ \f$\xi\mathcal{L}/c\f$
 * @param [in] m magnetic quantum number
 * @param [in] epsrel relative accuracy of integration
 * @retval integration object
 */
integration_t *caps_integrate_init(caps_t *caps, double xi_, int m, double epsrel)
{
    if(xi_ < 0 || m < 0 || epsrel <= 0)
        return NULL;

    integration_t *self = (integration_t *)xmalloc(sizeof(integration_t));

    self->caps = caps;
    self->m = m;
    self->alpha = 2*xi_;
    self->epsrel = epsrel;

    int elems = CAPS_CACHE_ELEMS;
    const char *s = getenv("CAPS_CACHE_ELEMS");
    if(s != NULL)
        elems = atoi(s);

    self->cache_I = cache_new(elems);

    self->elems_cache_K = 5*(caps->ldim+2*m+100);
    self->cache_K[0] = xmalloc(self->elems_cache_K*sizeof(double));
    self->cache_K[1] = xmalloc(self->elems_cache_K*sizeof(double));
    for(size_t i = 0; i < self->elems_cache_K; i++)
    {
        self->cache_K[0][i] = NAN;
        self->cache_K[1][i] = NAN;
    }

    /* determine whether we have perfect reflectors or not */
    if(isinf(caps_epsilonm1_plate(caps, INFINITY)))
        self->is_pr = true;
    else
        self->is_pr = false;

    return self;
}


/** @brief Free integration object
 *
 * @param [in,out] integration integration object
 */
void caps_integrate_free(integration_t *integration)
{
    if(integration != NULL)
    {
        cache_free(integration->cache_I);
        xfree(integration->cache_K[0]);
        xfree(integration->cache_K[1]);
        xfree(integration);
    }
}

/** Compute integral \f$A_{\ell_1,\ell_2,p}^{(m)}(\xi)\f$
 *
 * Compute the integral
 * \f[
 * A_{\ell_1,\ell_2,p}^{(m)}(\xi) = \frac{m^2 \xi}{c} \int_0^\infty  \mathrm{d}k \frac{r_p}{k\kappa} e^{-2\kappa\mathcal{L}} P_{\ell_1}^m\left(\frac{\kappa c}{\xi}\right) P_{\ell_2}^m\left(\frac{\kappa c}{\xi}\right)
 * \f]
 *
 * @param [in] self integration object
 * @param [in] l1 parameter
 * @param [in] l2 parameter
 * @param [in] p polarization; either TE or TM
 * @param [out] sign sign of integral \f$\mathrm{sgn}\left(A_{\ell_1,\ell_2,p}^{(m)}(\xi)\right)\f$
 * @retval logA \f$\log\left|A_{\ell_1,\ell_2,p}^{(m)}(\xi)\right|\f$
 */
double caps_integrate_A(integration_t *self, int l1, int l2, polarization_t p, sign_t *sign)
{
    const int m = self->m;

    if(m == 0)
    {
        *sign = 0;
        return -INFINITY;
    }

    const double I1 = caps_integrate_I(self, l1, l2, p, sign);
    const double A0 = 2*logi(m);

    const double A = A0+I1;
    TERMINATE(!isfinite(A), "l1=%d, l2=%d, m=%d, p=%d, I1=%g, A0=%g, A=%g", l1,l2,m,p, I1, A0, A);
    return A;
}

/** Compute integral \f$B_{\ell_1,\ell_2,p}^{(m)}(\xi)\f$
 *
 * Compute the integral
 * \f[
 * B_{\ell_1,\ell_2,p}^{(m)}(\xi) = \frac{c^3}{\xi^3} \int_0^\infty  \mathrm{d}k \frac{k^3}{\kappa} r_p e^{-2\kappa\mathcal{L}} {P_{\ell_1}^m}^\prime\left(\frac{\kappa c}{\xi}\right) {P_{\ell_2}^m}^\prime\left(\frac{\kappa c}{\xi}\right)
 * \f]
 *
 * @param [in] self integration object
 * @param [in] l1 parameter
 * @param [in] l2 parameter
 * @param [in] p polarization; either TE or TM
 * @param [out] sign sign of integral \f$\mathrm{sgn}\left(B_{\ell_1,\ell_2,p}^{(m)}(\xi)\right)\f$
 * @retval logB \f$\log\left|B_{\ell_1,\ell_2,p}^{(m)}(\xi)\right|\f$
 */
double caps_integrate_B(integration_t *self, int l1, int l2, polarization_t p, sign_t *sign)
{
    const int m = self->m;

    if(m == 0)
    {
        const double B = caps_integrate_I(self, l1, l2, p, sign);

        TERMINATE(!isfinite(B), "l1=%d, l2=%d, m=%d, p=%d, B=%g", l1,l2,m,p,B);

        return B;
    }

    sign_t sign1, sign2, sign3, sign4;
    const double I1 = caps_integrate_I(self, l1-1, l2-1, p, &sign1);
    const double I2 = caps_integrate_I(self, l1+1, l2-1, p, &sign2);
    const double I3 = caps_integrate_I(self, l1-1, l2+1, p, &sign3);
    const double I4 = caps_integrate_I(self, l1+1, l2+1, p, &sign4);

    double I;
    const double denom = (2*l1+1.)*(2*l2+1.);
    const double maximum = fmax(fmax(I1,I2), fmax(I3,I4));
    I  = (l1+1.)*(l1+m)*(l2+1.)*(l2+m)/denom*sign1*exp(I1-maximum);
    I -=   l1*(l1-m+1.)*(l2+1.)*(l2+m)/denom*sign2*exp(I2-maximum);
    I -=   (l1+1.)*(l1+m)*l2*(l2-m+1.)/denom*sign3*exp(I3-maximum);
    I +=     l1*(l1-m+1.)*l2*(l2-m+1.)/denom*sign4*exp(I4-maximum);

    *sign = SGN(I);

    const double B = maximum+log(fabs(I));
    TERMINATE(!isfinite(B), "l1=%d, l2=%d, m=%d, p=%d, I1=%g, I2=%g, I3=%g, I4=%g, B=%g", l1,l2,m,p, I1,I2,I3,I4, B);
    return B;
}

/** Compute integral \f$C_{\ell_1,\ell_2,p}^{(m)}(\xi)\f$
 *
 * Compute the integral
 * \f[
 * C_{\ell_1,\ell_2,p}^{(m)}(\xi) = \frac{mc}{\xi} \int_0^\infty \mathrm{d}k \frac{k}{\kappa} r_p e^{-2\kappa\mathcal{L}} P_{\ell_1}^m\left(\frac{\kappa c}{\xi}\right) {P_{\ell_2}^m}^\prime\left(\frac{\kappa c}{\xi}\right)
 * \f]
 *
 * @param [in] self integration object
 * @param [in] l1 parameter
 * @param [in] l2 parameter
 * @param [in] p polarization; either TE or TM
 * @param [out] sign sign of integral \f$\mathrm{sgn}\left(C_{\ell_1,\ell_2,p}^{(m)}(\xi)\right)\f$
 * @retval logC \f$\log\left|C_{\ell_1,\ell_2,p}^{(m)}(\xi)\right|\f$
 */
double caps_integrate_C(integration_t *self, int l1, int l2, polarization_t p, sign_t *sign)
{
    const int m = self->m;
    if(m == 0)
    {
        *sign = 0;
        return -INFINITY;
    }

    const double C0 = logi(m);

    sign_t sign1, sign2;
    const double I1 = caps_integrate_I(self, l1, l2-1, p, &sign1);
    const double I2 = caps_integrate_I(self, l1, l2+1, p, &sign2);

    const double denom = 2*l2+1;
    double I;
    I  = -(l2+1.)*(l2+m)/denom*sign1*exp(I1-I2);
    I += l2*(l2-m+1.)/denom*sign2;

    *sign = SGN(I);

    const double C = C0+I2+log(fabs(I));
    TERMINATE(!isfinite(C), "l1=%d, l2=%d, m=%d, p=%d, I1=%g, I2=%g, C0=%g, C=%g", l1,l2,m,p, I1,I2, C0, C);
    return C;
}

/** @brief Compute integral \f$D_{\ell_1,\ell_2,p}^{(m)}(\xi)\f$
 *
 * Compute
 * \f[
 * D_{\ell_1,\ell_2,p}^{(m)}(\xi) = C_{\ell_2,\ell_2,1}^{(m)}(\xi)
 * \f]
 *
 * This function calls \ref caps_integrate_C.
 *
 * @param [in] self integration object
 * @param [in] l1 parameter
 * @param [in] l2 parameter
 * @param [in] p polarization; either TE or TM
 * @param [out] sign sign of integral \f$\mathrm{sgn}\left(D_{\ell_1,\ell_2,p}^{(m)}(\xi)\right)\f$
 * @retval logD \f$\log\left|D_{\ell_1,\ell_2,p}^{(m)}(\xi)\right|\f$
 */
double caps_integrate_D(integration_t *self, int l1, int l2, polarization_t p, sign_t *sign)
{
    return caps_integrate_C(self, l2, l1, p, sign);
}

/** @brief Initialize integration object for plasma high temperature limit (\f$\xi=0\f$)
 *
 * @param [in] caps CaPS object
 * @param [in] omegap plasma frequency in rad/s
 * @param [in] epsrel relative error for integration
 * @retval self plasma integration object
 */
integration_plasma_t *caps_integrate_plasma_init(caps_t *caps, double omegap, double epsrel)
{
    integration_plasma_t *self;

    double omegap_ = omegap*caps->calL/CAPS_c;

    self = (integration_plasma_t *)xmalloc(sizeof(integration_plasma_t));
    self->LbyR    = caps->LbyR;
    self->omegap_ = omegap_;
    self->alpha   = omegap_/(1+caps->LbyR);
    self->epsrel  = epsrel;

    const int ldim = caps->ldim;
    self->cache       = cache_new(10*ldim);
    self->cache_ratio = cache_new(10*ldim);

    return self;
}

typedef struct {
    int nu;
    double omegap, log_prefactor;
} integrand_plasma_t;

static double _integrand_plasma(double t, void *args_)
{
    integrand_plasma_t *args = (integrand_plasma_t *)args_;

    const double betam1 = sqrtpm1(pow_2(2*args->omegap/t));
    const double rTE = -betam1/(2+betam1);

    return -rTE * exp(args->log_prefactor -t+args->nu*log(t));
}

/** @brief Compute integral for plasma high temperatures
 *
 * Compute the integral
 * \f[
 *      \int_0^\infty \mathrm{d}x \, x^{\ell_1+\ell_2} \mathrm{e}^{-x} r_\mathrm{TE}
 * \f]
 * where
 * \f[
 *      r_\mathrm{TE} = \frac{\sqrt{x^2+\beta^2}-x}{\sqrt{x^2+\beta^2}+x}
 * \f]
 * and \f$\beta=2\omega_\mathrm{P}(L+R)/c\f$.
 *
 * - If ratio1 is not NULL, ratio1 will be set to \f$I_{\ell_1-1/2}(\alpha)/I_{\ell_1+1/2}(\alpha)\f$ where \f$\alpha=2\xi(L+R)/c\f$.
 * - If ratio2 is not NULL, ratio2 will be set to \f$I_{\ell_2-1/2}(\alpha)/I_{\ell_2+1/2}(\alpha)\f$ where \f$\alpha=2\xi(L+R)/c\f$.
 *
 * @param [in] self plasma integration object
 * @param [in] l1 \f$\ell_1\f$
 * @param [in] l2 \f$\ell_2\f$
 * @param [in] m \f$m\f$
 * @param [out] ratio1
 * @param [out] ratio2
 * @retval I value of integral
 */
double caps_integrate_plasma(integration_plasma_t *self, int l1, int l2, int m, double *ratio1, double *ratio2)
{
    const int nu = l1+l2;

    /* ratio1 */
    *ratio1 = cache_lookup(self->cache_ratio, l1);
    if(isnan(*ratio1))
    {
        *ratio1 = bessel_ratioI(l1-0.5, self->alpha);
        cache_insert(self->cache_ratio, l1, *ratio1);
    }

    /* ratio2 */
    *ratio2 = cache_lookup(self->cache_ratio, l2);
    if(isnan(*ratio2))
    {
        *ratio2 = bessel_ratioI(l2-0.5, self->alpha);
        cache_insert(self->cache_ratio, l2, *ratio2);
    }

    double I = cache_lookup(self->cache, nu);
    if(!isnan(I))
        return I;

    /* compute integral
     *      1/prefactor * int_0^infty dz r_TE e^(-z) z^nu
     * with
     *      prefactor = nu!
     */

    /* find left and right boundaries */
    const double width = 1/sqrt(nu);
    const double xmax = nu;
    const double a = fmax(0, nu-5*width);
    const double b = xmax+5*width;

    /* perform integrations in intervals [0,a], [a,b] and [b,∞] */
    const double epsrel = self->epsrel;
    integrand_plasma_t args = { .nu = nu, .omegap = self->omegap_, .log_prefactor = -lfac(nu) };

    int neval1 = 0, neval2 = 0, neval3 = 0, ier1 = 0, ier2 = 0, ier3 = 0;
    double abserr1 = 0, abserr2 = 0, abserr3 = 0, I1 = 0, I2 = 0, I3 = 0;

    /* I2: [a,b] */
    I2 = dqags(_integrand_plasma, a, b, 0, epsrel, &abserr2, &neval2, &ier2, &args);

    /* I1: [0,a] */
    if(a > 0)
    {
        /* The contribution of this integral should be small, so use
         * Gauss-Kronrod G_K 7-15 as integration rule.
         */
        int limit = 200;
        I1 = dqage(_integrand_plasma, 0, a, abserr2, epsrel, GK_7_15, &abserr1, &neval1, &ier1, &limit, &args);
    }

    /* I3: [b,∞] */
    I3 = dqagi(_integrand_plasma, b, 1, abserr2, epsrel, &abserr3, &neval3, &ier3, &args);

    I = I1+I2+I3;
    bool warn = ier1 != 0 || ier2 != 0 || ier3 != 0 || isnan(I) || I == 0;
    WARN(warn, "ier1=%d, ier2=%d, ier3=%d, nu=%d, m=%d, a=%g, b=%g, I1=%g, I2=%g, I3=%g", ier1, ier2, ier3, nu,m,a,b, I1, I2, I3);

    cache_insert(self->cache, nu, I);

    return I;
}

/** @brief Free plasma integration object
 *
 * @param [in,out] self plasma integration object
 */
void caps_integrate_plasma_free(integration_plasma_t *self)
{
    cache_free(self->cache);
    cache_free(self->cache_ratio);
    xfree(self);
}
