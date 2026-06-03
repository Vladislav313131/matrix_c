#include "../s21_matrix.h"

int s21_mult_number(matrix_t *A, double number, matrix_t *result) {

    int flag = ((A != NULL && result != NULL && A->matrix != NULL ) ? OK : INCORRECT_MATRIX);

    if(flag != INCORRECT_MATRIX) {
        int matrix_res = s21_create_matrix(A->rows, A->columns, result);
        if(matrix_res != OK) {
            flag = matrix_res;
        } else {
            for(int i = 0; i < result->rows && flag == OK; i++) {
                for(int j = 0; j < result->columns && flag == OK; j++) {
                    result->matrix[i][j] = A->matrix[i][j] * number;
                    if(!isfinite(result->matrix[i][j])) {
                        matrix_res = INCORRECT_MATRIX;
                        break; // как правильно применить breake, чтобы выйти из цикла
                    } 
                }
            }
            flag = matrix_res;
            if(flag != OK) {
                s21_remove_matrix(result);
            }
        }
    }
    return flag;
}