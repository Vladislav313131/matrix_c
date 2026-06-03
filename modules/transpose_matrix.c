#include "../s21_matrix.h"

int s21_transpose(matrix_t *A, matrix_t *result) {

    int flag = (A == NULL || result == NULL || A->matrix == NULL) ? INCORRECT_MATRIX : OK;

    if(flag == OK) {
        int matrix_res = s21_create_matrix(A->rows, A->columns, result);
        if(matrix_res == OK) {
            for(int i = 0; i < result->rows; i++) {
                for(int j = 0; j < result->columns; j++) {
                    result->matrix[i][j] = A->matrix[j][i];
                }
            }
        }
    }
    return flag;
}