#include "../s21_matrix.h"

int s21_create_matrix(int rows, int columns, matrix_t *result) {
  int flag = OK;

  if (result != NULL) {
    result->matrix = NULL;
    result->rows = 0;
    result->columns = 0;
  }

  if (result == NULL || rows <= 0 || columns <= 0) {
    flag = INCORRECT_MATRIX;
  } else {
    result->matrix = calloc(rows, sizeof(double *));
    if (result->matrix == NULL) {
      flag = INCORRECT_MATRIX;
    } else {
      result->rows = rows;
      result->columns = columns;
      for (int i = 0; i < rows; i++) {
        result->matrix[i] = calloc(columns, sizeof(double));
        if (result->matrix[i] == NULL) {
          for (int j = 0; j < i; j++) {
            free(result->matrix[j]);
          }
          free(result->matrix);
          result->matrix = NULL;
          result->rows = 0;
          result->columns = 0;
          flag = CALC_ERROR;
          break;
        }
      }
    }
  }
  return flag;
}