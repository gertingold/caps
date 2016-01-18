#include <math.h>

#include "edouble.h"
#include "matrix.h"
#include "unittest.h"

#include "test_determinant.h"

int test_determinant()
{
    const char *methods[] = { "QR", "LU", NULL };
    unittest_t test;
    unittest_init(&test, "logdet", "Test computation of determinant");

    matrix_edouble_t *M      = matrix_edouble_alloc(2);
    matrix_sign_t    *M_sign = matrix_sign_alloc(2);

    matrix_set(M_sign, 0,0, +1);
    matrix_set(M_sign, 0,1, +1);
    matrix_set(M_sign, 1,0, +1);
    matrix_set(M_sign, 1,1, +1);

    for(int i = 0; methods[i] != NULL; i++)
    {
        const char *method = methods[i];
        for(int pnorm = 1; pnorm <= 2; pnorm++)
        {
            matrix_set(M, 0,0, loge(2L));
            matrix_set(M, 0,1, loge(20e1000L));
            matrix_set(M, 1,0, loge(1e-1000L));
            matrix_set(M, 1,1, loge(1L));

            AssertAlmostEqual(&test, matrix_edouble_logdet(M, M_sign, pnorm, method), log(18));
        }
    }

    matrix_edouble_free(M);
    matrix_sign_free(M_sign);

    return test_results(&test, stderr);
}
