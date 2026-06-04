#include "../s21_matrix.h"

static int s21_determinant_recursive(matrix_t *A, int n, double *result);

int s21_determinant(matrix_t *A, double *result) {
  int flag = OK;

  if (A == NULL || result == NULL || A->matrix == NULL || A->rows <= 0 ||
      A->columns <= 0) {
    flag = INCORRECT_MATRIX;

  } else if (A->rows != A->columns) {
    flag = CALC_ERROR;

  } else {
    flag = s21_determinant_recursive(A, A->rows, result);
  }

  return flag;
}

static int s21_determinant_recursive(matrix_t *A, int n, double *result) {
  int flag = OK;

  *result = 0.0;

  if (n == 1) {
    *result = A->matrix[0][0];

  } else if (n == 2) {
    *result =
        A->matrix[0][0] * A->matrix[1][1] - A->matrix[0][1] * A->matrix[1][0];

  } else {
    for (int j = 0; j < n && flag == OK; j++) {
      matrix_t minor = {0};
      double minor_det = 0.0;

      flag = s21_create_minor(A, &minor, 0, j);

      if (flag == OK) {
        flag = s21_determinant_recursive(&minor, n - 1, &minor_det);

        if (flag == OK) {
          double sign = (j % 2 == 0) ? 1.0 : -1.0;
          *result += sign * A->matrix[0][j] * minor_det;
        }

        s21_remove_matrix(&minor);
      }
    }
  }

  return flag;
}