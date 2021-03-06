#ifndef MATRIX_H
#define MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "utils.h"

typedef enum { DETALG_HODLR, DETALG_LU, DETALG_QR, DETALG_CHOLESKY } detalg_t;

/** define matrix type */
typedef struct {
    size_t dim;  /**< dimension of matrix */
    size_t dim2; /**< square of dimension of matrix */
    size_t lda;  /**< leading order */
    double *M;   /**< pointer to data */
} matrix_t;


/** macro to access matrix elements */
#define matrix_get(m, i, j)   ((m)->M[(i)*(m)->lda+(j)])

/** macro to set matrix elements */
#define matrix_set(m, i, j,v) ((m)->M[(i)*(m)->lda+(j)]=(v))


/* prototypes */
matrix_t *matrix_alloc(const size_t dim);

void matrix_free(matrix_t *A);
void matrix_setall(matrix_t *A, double z);

int matrix_save_to_stream(matrix_t *A, FILE *stream);
int matrix_save_to_file(matrix_t *A, const char *filename);

matrix_t *matrix_load_from_stream(FILE *stream);
matrix_t *matrix_load_from_file(const char *filename);

double matrix_trace(matrix_t *A);
double matrix_trace2(matrix_t *A);

double matrix_norm_frobenius(matrix_t *A);

double kernel_logdet(int dim, double (*M)(int,int,void *), void *args, int sym_spd, detalg_t detalg);

double matrix_logdet_triangular(matrix_t *A);
double matrix_logdet_dense(matrix_t *A, double z, detalg_t detalg);
double matrix_logdet_cholesky(matrix_t *A, char uplo);
double matrix_logdet_lu(matrix_t *A);
double matrix_logdet_qr(matrix_t *A);

matrix_t *matrix_mult(matrix_t *A, matrix_t *B, double alpha);

matrix_t *matrix_copy(matrix_t *A);

#ifdef __cplusplus
}
#endif

#endif
