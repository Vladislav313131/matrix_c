#include "../s21_matrix.h"

int s21_sub_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int flag =
      ((A != NULL && B != NULL && result != NULL)
           ? ((A->matrix != NULL && B->matrix != NULL) ? OK : INCORRECT_MATRIX)
           : INCORRECT_MATRIX);

  if (flag == INCORRECT_MATRIX || A->rows != B->rows ||
      A->columns != B->columns) {
    flag = INCORRECT_MATRIX;
  } else {
    int matrix_res = s21_create_matrix(A->rows, A->columns, result);
    if (matrix_res != OK) {
      flag = matrix_res;
    } else {
      for (int i = 0; i < result->rows && matrix_res == OK; i++) {
        for (int j = 0; j < result->columns && matrix_res == OK; j++) {
          result->matrix[i][j] = A->matrix[i][j] - B->matrix[i][j];
          if (!isfinite(result->matrix[i][j])) {
            matrix_res = CALC_ERROR;
          }
        }
      }
      flag = matrix_res;
    }
  }
  return flag;
}