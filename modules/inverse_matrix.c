#include "../s21_matrix.h"

int s21_inverse_matrix(matrix_t *A, matrix_t *result) {
  int flag = OK;
  double det = 0.0;

  if (A == NULL || result == NULL || A->matrix == NULL || A->rows <= 0 ||
      A->columns <= 0) {
    flag = INCORRECT_MATRIX;

  } else if (A->rows != A->columns) {
    flag = CALC_ERROR;

  } else {
    flag = s21_determinant(A, &det);

    if (flag == OK) {
      if (det > -1e-7 && det < 1e-7) {
        flag = CALC_ERROR;

      } else {
        matrix_t complements = {0};
        matrix_t transposed = {0};

        flag = s21_calc_complements(A, &complements);

        if (flag == OK) {
          flag = s21_transpose(&complements, &transposed);

          if (flag == OK) {
            flag = s21_mult_number(&transposed, 1.0 / det, result);

            s21_remove_matrix(&transposed);
          }

          s21_remove_matrix(&complements);
        }
      }
    }
  }

  return flag;
}