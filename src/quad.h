#ifndef __QUAD_H
#define __QUAD_H

#if defined(__ICC) || defined(__INTEL_COMPILER)
    #define __float128 _Quad

    /* deine isinf and isnan */
    #define isinfq(x) (x/10 == x)
    #define isnanq(x) (x != x)

    /* define prototypes. without these prototypes icc will return nan. */
    _Quad __logq(_Quad);
    _Quad __expq(_Quad);
    _Quad __sqrtq(_Quad);
    _Quad __log1pq(_Quad);

    #define logq __logq
    #define expq __expq
    #define sqrtq __sqrtq(_Quad);
    #define log1pq __log1pq(_Quad);
#elif defined(__GNUC__) || defined(__GNUG__)
    #include <quadmath.h>
#else
    #error "I'm sorry, but quad precision is only supported with gcc or icc at the moment."
#endif

#endif
