#include "../s21_matrix.h"

int s21_eq_matrix(matrix_t *A, matrix_t *B) {
  int flag = SUCCESS;

  if ((A == NULL || A->matrix == NULL) || (B == NULL || B->matrix == NULL)) {
    flag = FAILURE;
  } else if (A->rows != B->rows || A->columns != B->columns) {
    flag = FAILURE;
  } else {
    for (int i = 0; i < A->rows && flag == SUCCESS; i++) {
      for (int j = 0; j < A->columns && flag == SUCCESS; j++) {
        double check = A->matrix[i][j] - B->matrix[i][j];
        if (check < -1e-7 || check > 1e-6) {
          flag = FAILURE;
        }
      }
    }
  }
  return flag;
}