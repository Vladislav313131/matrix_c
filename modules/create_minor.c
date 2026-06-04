#include "../s21_matrix.h"

int s21_create_minor(matrix_t *A, matrix_t *minor, int row, int column) {
  int flag = OK;

  flag = s21_create_matrix(A->rows - 1, A->columns - 1, minor);

  if (flag == OK) {
    int minor_i = 0;

    for (int i = 0; i < A->rows; i++) {
      if (i != row) {
        int minor_j = 0;

        for (int j = 0; j < A->columns; j++) {
          if (j != column) {
            minor->matrix[minor_i][minor_j] = A->matrix[i][j];
            minor_j++;
          }
        }

        minor_i++;
      }
    }
  }

  return flag;
}