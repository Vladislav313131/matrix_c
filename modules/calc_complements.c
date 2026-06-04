#include "../s21_matrix.h"

int s21_calc_complements(matrix_t *A, matrix_t *result) {
  int flag = OK;

  if (A == NULL || result == NULL || A->matrix == NULL || A->rows <= 0 ||
      A->columns <= 0) {
    flag = INCORRECT_MATRIX;

  } else if (A->rows != A->columns) {
    flag = CALC_ERROR;

  } else {
    flag = s21_create_matrix(A->rows, A->columns, result);

    if (flag == OK) {
      if (A->rows == 1) {
        result->matrix[0][0] = 1.0;

      } else {
        for (int i = 0; i < A->rows && flag == OK; i++) {
          for (int j = 0; j < A->columns && flag == OK; j++) {
            matrix_t minor = {0};
            double det = 0.0;

            flag = s21_create_minor(A, &minor, i, j);

            if (flag == OK) {
              flag = s21_determinant(&minor, &det);

              if (flag == OK) {
                double sign = ((i + j) % 2 == 0) ? 1.0 : -1.0;
                result->matrix[i][j] = sign * det;
              }

              s21_remove_matrix(&minor);
            }
          }
        }

        if (flag != OK) {
          s21_remove_matrix(result);
        }
      }
    }
  }

  return flag;
}
