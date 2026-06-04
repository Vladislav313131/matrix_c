#include "../s21_matrix.h"

static double count_matrix(int i, int j, matrix_t *A, matrix_t *B);

int s21_mult_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int flag = OK;

  if (A == NULL || B == NULL || result == NULL || A->matrix == NULL ||
      B->matrix == NULL || A->rows <= 0 || A->columns <= 0 || B->rows <= 0 ||
      B->columns <= 0) {
    flag = INCORRECT_MATRIX;

  } else if (A->columns != B->rows) {
    flag = CALC_ERROR;

  } else {
    int matrix_res = s21_create_matrix(A->rows, B->columns, result);

    if (matrix_res != OK) {
      flag = matrix_res;

    } else {
      for (int i = 0; i < result->rows && flag == OK; i++) {
        for (int j = 0; j < result->columns && flag == OK; j++) {
          double value = count_matrix(i, j, A, B);

          if (isfinite(value)) {
            result->matrix[i][j] = value;
          } else {
            s21_remove_matrix(result);
            flag = CALC_ERROR;
          }
        }
      }
    }
  }

  return flag;
}

static double count_matrix(int i, int j, matrix_t *A, matrix_t *B) {
  double res = 0.0;

  for (int k = 0; k < A->columns; k++) {
    res += A->matrix[i][k] * B->matrix[k][j];
  }

  return res;
}