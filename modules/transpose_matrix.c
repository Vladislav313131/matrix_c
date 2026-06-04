#include "../s21_matrix.h"

int s21_transpose(matrix_t *A, matrix_t *result) {
  int flag = OK;

  if (A == NULL || result == NULL || A->matrix == NULL || A->rows <= 0 ||
      A->columns <= 0) {
    flag = INCORRECT_MATRIX;

  } else {
    flag = s21_create_matrix(A->columns, A->rows, result);

    if (flag == OK) {
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
          result->matrix[j][i] = A->matrix[i][j];
        }
      }
    }
  }

  return flag;
}