#include <check.h>
#include <float.h>
#include <stdlib.h>

#include "s21_matrix.h"

static int g_calloc_fail_enabled = 0;
static int g_calloc_call_count = 0;
static int g_calloc_fail_at = 0;

void *__real_calloc(size_t nmemb, size_t size);

void *__wrap_calloc(size_t nmemb, size_t size) {
  g_calloc_call_count++;

  if (g_calloc_fail_enabled && g_calloc_call_count == g_calloc_fail_at) {
    return NULL;
  }

  return __real_calloc(nmemb, size);
}

static void reset_calloc_mock(void) {
  g_calloc_fail_enabled = 0;
  g_calloc_call_count = 0;
  g_calloc_fail_at = 0;
}

static void fill_3x3(matrix_t *A, double a00, double a01, double a02,
                     double a10, double a11, double a12, double a20, double a21,
                     double a22) {
  A->matrix[0][0] = a00;
  A->matrix[0][1] = a01;
  A->matrix[0][2] = a02;
  A->matrix[1][0] = a10;
  A->matrix[1][1] = a11;
  A->matrix[1][2] = a12;
  A->matrix[2][0] = a20;
  A->matrix[2][1] = a21;
  A->matrix[2][2] = a22;
}

START_TEST(test_create_and_remove) {
  matrix_t matrix_a = {0};
  ck_assert_int_eq(s21_create_matrix(3, 3, &matrix_a), OK);
  ck_assert_int_eq(matrix_a.rows, 3);
  ck_assert_int_eq(matrix_a.columns, 3);
  ck_assert_ptr_nonnull(matrix_a.matrix);

  s21_remove_matrix(&matrix_a);
  ck_assert_ptr_null(matrix_a.matrix);
  ck_assert_int_eq(matrix_a.rows, 0);
  ck_assert_int_eq(matrix_a.columns, 0);
}
END_TEST

START_TEST(test_remove_null_and_empty) {
  matrix_t matrix_a = {0};

  s21_remove_matrix(NULL);
  s21_remove_matrix(&matrix_a);
  ck_assert_ptr_null(matrix_a.matrix);
}
END_TEST

START_TEST(test_create_invalid) {
  matrix_t matrix_a = {0};
  ck_assert_int_eq(s21_create_matrix(0, 3, &matrix_a), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_create_matrix(3, -1, &matrix_a), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_create_matrix(3, 3, NULL), INCORRECT_MATRIX);
}
END_TEST

START_TEST(test_create_matrix_main_calloc_fail) {
  matrix_t result = {0};

  reset_calloc_mock();
  g_calloc_fail_enabled = 1;
  g_calloc_call_count = 0;
  g_calloc_fail_at = 1;

  int status = s21_create_matrix(3, 3, &result);

  g_calloc_fail_enabled = 0;
  g_calloc_call_count = 0;
  g_calloc_fail_at = 0;

  ck_assert_int_eq(status, INCORRECT_MATRIX);
  ck_assert_ptr_null(result.matrix);
  ck_assert_int_eq(result.rows, 0);
  ck_assert_int_eq(result.columns, 0);

  reset_calloc_mock();
}
END_TEST

START_TEST(test_create_matrix_row_calloc_fail) {
  matrix_t result = {0};

  reset_calloc_mock();
  g_calloc_fail_enabled = 1;
  g_calloc_call_count = 0;
  g_calloc_fail_at = 2;

  int status = s21_create_matrix(3, 3, &result);

  g_calloc_fail_enabled = 0;
  g_calloc_call_count = 0;
  g_calloc_fail_at = 0;

  ck_assert_int_eq(status, CALC_ERROR);
  ck_assert_ptr_null(result.matrix);
  ck_assert_int_eq(result.rows, 0);
  ck_assert_int_eq(result.columns, 0);
}
END_TEST

START_TEST(test_eq_matrix_success_with_precision) {
  matrix_t a = {0};
  matrix_t b = {0};
  s21_create_matrix(2, 2, &a);
  s21_create_matrix(2, 2, &b);

  a.matrix[0][0] = 1.1234561;
  b.matrix[0][0] = 1.1234562;
  a.matrix[1][1] = -5.0;
  b.matrix[1][1] = -5.0;

  ck_assert_int_eq(s21_eq_matrix(&a, &b), SUCCESS);

  s21_remove_matrix(&a);
  s21_remove_matrix(&b);
}
END_TEST

START_TEST(test_eq_matrix_failure_cases) {
  matrix_t a = {0};
  matrix_t b = {0};
  matrix_t c = {0};
  s21_create_matrix(2, 2, &a);
  s21_create_matrix(2, 2, &b);
  s21_create_matrix(2, 3, &c);

  a.matrix[0][0] = 1.0;
  b.matrix[0][0] = 1.00001;

  ck_assert_int_eq(s21_eq_matrix(NULL, &b), FAILURE);
  ck_assert_int_eq(s21_eq_matrix(&a, NULL), FAILURE);
  ck_assert_int_eq(s21_eq_matrix(&a, &c), FAILURE);
  ck_assert_int_eq(s21_eq_matrix(&a, &b), FAILURE);

  s21_remove_matrix(&a);
  s21_remove_matrix(&b);
  s21_remove_matrix(&c);
}
END_TEST

START_TEST(test_sum_matrix_ok) {
  matrix_t a = {0};
  matrix_t b = {0};
  matrix_t r = {0};
  s21_create_matrix(2, 2, &a);
  s21_create_matrix(2, 2, &b);

  a.matrix[0][0] = 1.0;
  a.matrix[0][1] = 2.0;
  a.matrix[1][0] = 3.0;
  a.matrix[1][1] = 4.0;
  b.matrix[0][0] = 5.0;
  b.matrix[0][1] = 6.0;
  b.matrix[1][0] = 7.0;
  b.matrix[1][1] = 8.0;

  ck_assert_int_eq(s21_sum_matrix(&a, &b, &r), OK);
  ck_assert_double_eq_tol(r.matrix[0][0], 6.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[0][1], 8.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[1][0], 10.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[1][1], 12.0, 1e-6);

  s21_remove_matrix(&a);
  s21_remove_matrix(&b);
  s21_remove_matrix(&r);
}
END_TEST

START_TEST(test_sub_matrix_ok) {
  matrix_t a = {0};
  matrix_t b = {0};
  matrix_t r = {0};
  s21_create_matrix(2, 2, &a);
  s21_create_matrix(2, 2, &b);

  a.matrix[0][0] = 5.0;
  a.matrix[0][1] = 6.0;
  a.matrix[1][0] = 7.0;
  a.matrix[1][1] = 8.0;
  b.matrix[0][0] = 1.0;
  b.matrix[0][1] = 2.0;
  b.matrix[1][0] = 3.0;
  b.matrix[1][1] = 4.0;

  ck_assert_int_eq(s21_sub_matrix(&a, &b, &r), OK);
  ck_assert_double_eq_tol(r.matrix[0][0], 4.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[0][1], 4.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[1][0], 4.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[1][1], 4.0, 1e-6);

  s21_remove_matrix(&a);
  s21_remove_matrix(&b);
  s21_remove_matrix(&r);
}
END_TEST

START_TEST(test_sum_sub_invalid_and_size_error) {
  matrix_t a = {0};
  matrix_t b = {0};
  matrix_t r = {0};
  s21_create_matrix(2, 2, &a);
  s21_create_matrix(3, 2, &b);

  ck_assert_int_eq(s21_sum_matrix(NULL, &b, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_sub_matrix(&a, NULL, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_sum_matrix(&a, &b, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_sub_matrix(&a, &b, &r), INCORRECT_MATRIX);

  s21_remove_matrix(&a);
  s21_remove_matrix(&b);
}
END_TEST

START_TEST(test_mult_number_ok) {
  matrix_t a = {0};
  matrix_t r = {0};
  s21_create_matrix(2, 2, &a);
  a.matrix[0][0] = 2.0;
  a.matrix[0][1] = -3.0;
  a.matrix[1][0] = 4.0;
  a.matrix[1][1] = 0.5;

  ck_assert_int_eq(s21_mult_number(&a, 3.0, &r), OK);
  ck_assert_double_eq_tol(r.matrix[0][0], 6.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[0][1], -9.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[1][0], 12.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[1][1], 1.5, 1e-6);

  s21_remove_matrix(&a);
  s21_remove_matrix(&r);
}
END_TEST

START_TEST(test_mult_number_invalid) {
  matrix_t a = {0};
  matrix_t r = {0};
  s21_create_matrix(2, 2, &a);

  ck_assert_int_eq(s21_mult_number(NULL, 2.0, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_mult_number(&a, 2.0, NULL), INCORRECT_MATRIX);

  s21_remove_matrix(&a);
}
END_TEST

START_TEST(test_mult_number_create_matrix_fail) {
  matrix_t a = {0};
  matrix_t result = {0};

  reset_calloc_mock();
  ck_assert_int_eq(s21_create_matrix(2, 2, &a), OK);

  reset_calloc_mock();
  g_calloc_fail_enabled = 1;
  g_calloc_fail_at = 1;

  int status = s21_mult_number(&a, 2.0, &result);

  reset_calloc_mock();

  ck_assert_int_eq(status, INCORRECT_MATRIX);
  ck_assert_ptr_null(result.matrix);

  s21_remove_matrix(&a);
}
END_TEST

START_TEST(test_mult_number_not_finite_result) {
  matrix_t a = {0};
  matrix_t result = {0};

  reset_calloc_mock();
  ck_assert_int_eq(s21_create_matrix(1, 1, &a), OK);
  a.matrix[0][0] = DBL_MAX;

  int status = s21_mult_number(&a, 2.0, &result);

  ck_assert_int_eq(status, INCORRECT_MATRIX);
  ck_assert_ptr_null(result.matrix);
  ck_assert_int_eq(result.rows, 0);
  ck_assert_int_eq(result.columns, 0);

  s21_remove_matrix(&a);
}
END_TEST

START_TEST(test_mult_matrix_ok) {
  matrix_t a = {0};
  matrix_t b = {0};
  matrix_t r = {0};
  s21_create_matrix(2, 3, &a);
  s21_create_matrix(3, 2, &b);

  a.matrix[0][0] = 1.0;
  a.matrix[0][1] = 2.0;
  a.matrix[0][2] = 3.0;
  a.matrix[1][0] = 4.0;
  a.matrix[1][1] = 5.0;
  a.matrix[1][2] = 6.0;

  b.matrix[0][0] = 7.0;
  b.matrix[0][1] = 8.0;
  b.matrix[1][0] = 9.0;
  b.matrix[1][1] = 10.0;
  b.matrix[2][0] = 11.0;
  b.matrix[2][1] = 12.0;

  ck_assert_int_eq(s21_mult_matrix(&a, &b, &r), OK);
  ck_assert_int_eq(r.rows, 2);
  ck_assert_int_eq(r.columns, 2);
  ck_assert_double_eq_tol(r.matrix[0][0], 58.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[0][1], 64.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[1][0], 139.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[1][1], 154.0, 1e-6);

  s21_remove_matrix(&a);
  s21_remove_matrix(&b);
  s21_remove_matrix(&r);
}
END_TEST

START_TEST(test_mult_matrix_invalid_and_size_error) {
  matrix_t a = {0};
  matrix_t b = {0};
  matrix_t r = {0};
  s21_create_matrix(2, 3, &a);
  s21_create_matrix(2, 2, &b);

  ck_assert_int_eq(s21_mult_matrix(NULL, &b, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_mult_matrix(&a, NULL, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_mult_matrix(&a, &b, NULL), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_mult_matrix(&a, &b, &r), CALC_ERROR);

  s21_remove_matrix(&a);
  s21_remove_matrix(&b);
}
END_TEST

START_TEST(test_transpose_ok) {
  matrix_t a = {0};
  matrix_t r = {0};
  s21_create_matrix(2, 3, &a);
  a.matrix[0][0] = 1.0;
  a.matrix[0][1] = 2.0;
  a.matrix[0][2] = 3.0;
  a.matrix[1][0] = 4.0;
  a.matrix[1][1] = 5.0;
  a.matrix[1][2] = 6.0;

  ck_assert_int_eq(s21_transpose(&a, &r), OK);
  ck_assert_int_eq(r.rows, 3);
  ck_assert_int_eq(r.columns, 2);
  ck_assert_double_eq_tol(r.matrix[0][0], 1.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[0][1], 4.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[1][0], 2.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[1][1], 5.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[2][0], 3.0, 1e-6);
  ck_assert_double_eq_tol(r.matrix[2][1], 6.0, 1e-6);

  s21_remove_matrix(&a);
  s21_remove_matrix(&r);
}
END_TEST

START_TEST(test_transpose_invalid) {
  matrix_t a = {0};
  matrix_t r = {0};
  s21_create_matrix(2, 2, &a);

  ck_assert_int_eq(s21_transpose(NULL, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_transpose(&a, NULL), INCORRECT_MATRIX);

  s21_remove_matrix(&a);
}
END_TEST

START_TEST(test_determinant_1x1) {
  matrix_t a = {0};
  double result = 0.0;
  s21_create_matrix(1, 1, &a);
  a.matrix[0][0] = 5.0;

  ck_assert_int_eq(s21_determinant(&a, &result), OK);
  ck_assert_double_eq_tol(result, 5.0, 1e-6);

  s21_remove_matrix(&a);
}
END_TEST

START_TEST(test_determinant_2x2) {
  matrix_t a = {0};
  double result = 0.0;
  s21_create_matrix(2, 2, &a);
  a.matrix[0][0] = 1.0;
  a.matrix[0][1] = 2.0;
  a.matrix[1][0] = 3.0;
  a.matrix[1][1] = 4.0;

  ck_assert_int_eq(s21_determinant(&a, &result), OK);
  ck_assert_double_eq_tol(result, -2.0, 1e-6);

  s21_remove_matrix(&a);
}
END_TEST

START_TEST(test_determinant_3x3) {
  matrix_t a = {0};
  double result = 0.0;
  s21_create_matrix(3, 3, &a);
  fill_3x3(&a, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);

  ck_assert_int_eq(s21_determinant(&a, &result), OK);
  ck_assert_double_eq_tol(result, 0.0, 1e-6);

  s21_remove_matrix(&a);
}
END_TEST

START_TEST(test_determinant_invalid_and_not_square) {
  matrix_t a = {0};
  double result = 0.0;
  s21_create_matrix(2, 3, &a);

  ck_assert_int_eq(s21_determinant(NULL, &result), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_determinant(&a, NULL), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_determinant(&a, &result), CALC_ERROR);

  s21_remove_matrix(&a);
}
END_TEST

START_TEST(test_calc_complements_1x1) {
  matrix_t a = {0};
  matrix_t result = {0};
  s21_create_matrix(1, 1, &a);
  a.matrix[0][0] = 5.0;

  ck_assert_int_eq(s21_calc_complements(&a, &result), OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 1.0, 1e-6);

  s21_remove_matrix(&a);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_calc_complements_3x3) {
  matrix_t a = {0};
  matrix_t result = {0};
  s21_create_matrix(3, 3, &a);
  fill_3x3(&a, 1.0, 2.0, 3.0, 0.0, 4.0, 2.0, 5.0, 2.0, 1.0);

  ck_assert_int_eq(s21_calc_complements(&a, &result), OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 0.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[0][1], 10.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[0][2], -20.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[1][0], 4.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[1][1], -14.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[1][2], 8.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[2][0], -8.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[2][1], -2.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[2][2], 4.0, 1e-6);

  s21_remove_matrix(&a);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_calc_complements_invalid_and_not_square) {
  matrix_t a = {0};
  matrix_t result = {0};
  s21_create_matrix(2, 3, &a);

  ck_assert_int_eq(s21_calc_complements(NULL, &result), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_calc_complements(&a, NULL), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_calc_complements(&a, &result), CALC_ERROR);

  s21_remove_matrix(&a);
}
END_TEST

START_TEST(test_inverse_matrix_1x1) {
  matrix_t a = {0};
  matrix_t result = {0};
  s21_create_matrix(1, 1, &a);
  a.matrix[0][0] = 5.0;

  ck_assert_int_eq(s21_inverse_matrix(&a, &result), OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 0.2, 1e-6);

  s21_remove_matrix(&a);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_inverse_matrix_3x3) {
  matrix_t a = {0};
  matrix_t result = {0};
  s21_create_matrix(3, 3, &a);
  fill_3x3(&a, 2.0, 5.0, 7.0, 6.0, 3.0, 4.0, 5.0, -2.0, -3.0);

  ck_assert_int_eq(s21_inverse_matrix(&a, &result), OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 1.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[0][1], -1.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[0][2], 1.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[1][0], -38.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[1][1], 41.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[1][2], -34.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[2][0], 27.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[2][1], -29.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[2][2], 24.0, 1e-6);

  s21_remove_matrix(&a);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_inverse_matrix_invalid_not_square_zero_det) {
  matrix_t not_square = {0};
  matrix_t zero_det = {0};
  matrix_t result = {0};
  s21_create_matrix(2, 3, &not_square);
  s21_create_matrix(3, 3, &zero_det);
  fill_3x3(&zero_det, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);

  ck_assert_int_eq(s21_inverse_matrix(NULL, &result), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_inverse_matrix(&not_square, &result), CALC_ERROR);
  ck_assert_int_eq(s21_inverse_matrix(&zero_det, &result), CALC_ERROR);

  s21_remove_matrix(&not_square);
  s21_remove_matrix(&zero_det);
}
END_TEST

Suite *matrix_suite(void) {
  Suite *s = suite_create("s21_matrix");
  TCase *tc = tcase_create("core");

  tcase_add_test(tc, test_create_and_remove);
  tcase_add_test(tc, test_remove_null_and_empty);
  tcase_add_test(tc, test_create_invalid);
  tcase_add_test(tc, test_create_matrix_main_calloc_fail);
  tcase_add_test(tc, test_create_matrix_row_calloc_fail);
  tcase_add_test(tc, test_eq_matrix_success_with_precision);
  tcase_add_test(tc, test_eq_matrix_failure_cases);
  tcase_add_test(tc, test_sum_matrix_ok);
  tcase_add_test(tc, test_sub_matrix_ok);
  tcase_add_test(tc, test_sum_sub_invalid_and_size_error);
  tcase_add_test(tc, test_mult_number_ok);
  tcase_add_test(tc, test_mult_number_invalid);
  tcase_add_test(tc, test_mult_number_create_matrix_fail);
  tcase_add_test(tc, test_mult_number_not_finite_result);
  tcase_add_test(tc, test_mult_matrix_ok);
  tcase_add_test(tc, test_mult_matrix_invalid_and_size_error);
  tcase_add_test(tc, test_transpose_ok);
  tcase_add_test(tc, test_transpose_invalid);
  tcase_add_test(tc, test_determinant_1x1);
  tcase_add_test(tc, test_determinant_2x2);
  tcase_add_test(tc, test_determinant_3x3);
  tcase_add_test(tc, test_determinant_invalid_and_not_square);
  tcase_add_test(tc, test_calc_complements_1x1);
  tcase_add_test(tc, test_calc_complements_3x3);
  tcase_add_test(tc, test_calc_complements_invalid_and_not_square);
  tcase_add_test(tc, test_inverse_matrix_1x1);
  tcase_add_test(tc, test_inverse_matrix_3x3);
  tcase_add_test(tc, test_inverse_matrix_invalid_not_square_zero_det);

  suite_add_tcase(s, tc);
  return s;
}

int main(void) {
  Suite *s = matrix_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int failed = srunner_ntests_failed(sr);

  srunner_free(sr);
  return failed == 0 ? 0 : 1;
}
