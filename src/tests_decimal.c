#include <check.h>

#include "s21_decimal.h"

#define UINT_MAX 4294967295

///// CONVERTERS - int to dec
START_TEST(test_s21_from_int_to_decimal_positive) {
  s21_decimal result;
  int src = 123;
  int flag = s21_from_int_to_decimal(src, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_uint_eq(result.bits[0], 123);
  ck_assert_int_eq(result.bits[3] >> 31, 0);
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_negative) {
  s21_decimal result;
  int src = -123;
  int flag = s21_from_int_to_decimal(src, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_uint_eq(result.bits[0], 123);
  ck_assert_int_eq(result.bits[3] >> 31, 1);
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_zero) {
  s21_decimal result;
  int src = 0;
  int flag = s21_from_int_to_decimal(src, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[3] >> 31, 0);
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_max_int) {
  s21_decimal result;
  int src = 2147483647;
  int flag = s21_from_int_to_decimal(src, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_uint_eq(result.bits[0], 2147483647);
  ck_assert_int_eq(result.bits[3] >> 31, 0);
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_min_int) {
  s21_decimal result;
  int src = -2147483647;
  int flag = s21_from_int_to_decimal(src, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_uint_eq(result.bits[0], 2147483647);
  ck_assert_int_eq(result.bits[3] >> 31, 1);
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_overflow_positive) {
  s21_decimal result;
  long int src = 2147483648;
  int flag = s21_from_int_to_decimal(src, &result);
  ck_assert_int_eq(flag, 1);
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_overflow_negative) {
  s21_decimal result;
  int src = -2147483648;
  int flag = s21_from_int_to_decimal(src, &result);
  ck_assert_int_eq(flag, 1);
}
END_TEST

START_TEST(test_s21_from_int_to_decimal_invalid_pointer) {
  int src = 123;
  int flag = s21_from_int_to_decimal(src, NULL);
  ck_assert_int_eq(flag, 1);
}
END_TEST

//////////////////////////// dec to int

START_TEST(test_normal_values) {
  s21_decimal src;
  int dst;
  src.bits[0] = 123;
  src.bits[1] = 0;
  src.bits[2] = 0;
  src.bits[3] = 0;  // scale = 0, sign = 0

  int result = s21_from_decimal_to_int(src, &dst);
  ck_assert_int_eq(result, 0);
  ck_assert_int_eq(dst, 123);
}
END_TEST

START_TEST(test_negative_value) {
  s21_decimal src;
  int dst;
  src.bits[0] = 123;
  src.bits[1] = 0;
  src.bits[2] = 0;
  src.bits[3] = 0x80000000;  // scale = 0, sign = 1

  int result = s21_from_decimal_to_int(src, &dst);
  ck_assert_int_eq(result, 0);
  ck_assert_int_eq(dst, -123);
}
END_TEST

START_TEST(test_scaling) {
  s21_decimal src;
  int dst;
  src.bits[0] = 123456000;
  src.bits[1] = 0;
  src.bits[2] = 0;
  src.bits[3] = 0x00030000;  // scale = 3, sign = 0

  int result = s21_from_decimal_to_int(src, &dst);
  ck_assert_int_eq(result, 0);
  ck_assert_int_eq(dst, 123456);
}
END_TEST

START_TEST(test_large_scale) {
  s21_decimal src;
  int dst;
  src.bits[0] = 1;
  src.bits[1] = 0;
  src.bits[2] = 0;
  src.bits[3] = 0x70000000;  // scale = 112, sign = 0

  int result = s21_from_decimal_to_int(src, &dst);
  ck_assert_int_eq(result, 1);
}
END_TEST

START_TEST(test_null_pointer) {
  s21_decimal src;
  src.bits[0] = 1;
  src.bits[1] = 0;
  src.bits[2] = 0;
  src.bits[3] = 0x00000000;  // scale = 0, sign = 0

  int result = s21_from_decimal_to_int(src, NULL);
  ck_assert_int_eq(result, 1);
}
END_TEST

////////////////////// float to dec

START_TEST(test_null_dst) {
  int flag = s21_from_float_to_decimal(3.14f, NULL);
  ck_assert_int_eq(flag, 1);
}
END_TEST

START_TEST(test_small_float) {
  s21_decimal result;
  int flag = s21_from_float_to_decimal(1e-29f, &result);
  ck_assert_int_eq(flag, 1);
}
END_TEST

START_TEST(test_large_float) {
  s21_decimal result;
  int flag = s21_from_float_to_decimal(8e28f, &result);
  ck_assert_int_eq(flag, 1);
}
END_TEST

START_TEST(test_infinity) {
  s21_decimal result;
  int flag = s21_from_float_to_decimal(INFINITY, &result);
  ck_assert_int_eq(flag, 1);
}
END_TEST

START_TEST(test_negative_float) {
  s21_decimal result;
  int flag = s21_from_float_to_decimal(-123.456f, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_int_eq(result.bits[3] >> 31, 1);
}
END_TEST

START_TEST(test_positive_float) {
  s21_decimal result;
  int flag = s21_from_float_to_decimal(123.456f, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_int_eq(result.bits[3] >> 31, 0);
}
END_TEST

START_TEST(test_small_positive) {
  s21_decimal result;
  int flag = s21_from_float_to_decimal(1.234567f, &result);
  ck_assert_int_eq(flag, 0);
}
END_TEST

START_TEST(test_small_negative) {
  s21_decimal result;
  int flag = s21_from_float_to_decimal(-1.234567f, &result);
  ck_assert_int_eq(flag, 0);
}
END_TEST

START_TEST(test_high_precision) {
  s21_decimal result;
  int flag = s21_from_float_to_decimal(1234567.1234567f, &result);
  ck_assert_int_eq(flag, 0);
}
END_TEST

START_TEST(test_normal_value) {
  s21_decimal result;
  int flag = s21_from_float_to_decimal(3.1415926f, &result);
  ck_assert_int_eq(flag, 0);
}
END_TEST

///////////// dec to float

START_TEST(test_dec_to_float_zero) {
  s21_decimal src = {{0, 0, 0, 0}};
  float dst = 1;
  int result = s21_from_decimal_to_float(src, &dst);
  ck_assert_int_eq(result, 0);
  ck_assert_float_eq(dst, 0.0f);
}
END_TEST

START_TEST(test_dec_to_float_small_positive) {
  s21_decimal src = {{123456, 0, 0, 0}};
  float dst;
  int result = s21_from_decimal_to_float(src, &dst);
  ck_assert_int_eq(result, 0);
  ck_assert_float_eq_tol(dst, 123456.0f, 1e-6);
}
END_TEST

START_TEST(test_dec_to_float_small_negative) {
  s21_decimal src = {{123456, 0, 0, 1 << 31}};
  float dst;
  int result = s21_from_decimal_to_float(src, &dst);
  ck_assert_int_eq(result, 0);
  ck_assert_float_eq_tol(dst, -123456.0f, 1e-6);
}
END_TEST

START_TEST(test_dec_to_float_large_positive) {
  s21_decimal src = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  float dst;
  int result = s21_from_decimal_to_float(src, &dst);
  ck_assert_int_eq(result, 0);
}
END_TEST

START_TEST(test_dec_to_float_large_negative) {
  s21_decimal src = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 1 << 31}};
  float dst;
  int result = s21_from_decimal_to_float(src, &dst);
  ck_assert_int_eq(result, 0);
}
END_TEST

START_TEST(test_dec_to_float_scale_reduction) {
  s21_decimal src = {{123456, 0, 0, 1 << 16}};  // scale = 1
  float dst;
  int result = s21_from_decimal_to_float(src, &dst);
  ck_assert_int_eq(result, 0);
  ck_assert_float_eq_tol(dst, 12345.6f, 1e-6);
}
END_TEST

START_TEST(test_dec_to_float_overflow) {
  s21_decimal src = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  float dst;
  int result = s21_from_decimal_to_float(src, &dst);
  ck_assert_int_eq(result, 0);
}
END_TEST

START_TEST(test_dec_to_float_underflow) {
  s21_decimal src = {{1, 0, 0, 1 << 16}};  // scale = 1
  float dst;
  int result = s21_from_decimal_to_float(src, &dst);
  ck_assert_int_eq(result, 0);
  ck_assert_float_eq(dst, 0.1f);
}
END_TEST

START_TEST(test_dec_to_float_exact_fit) {
  s21_decimal src = {{100000000, 0, 0, 1 << 16}};  // scale = 8
  float dst;
  int result = s21_from_decimal_to_float(src, &dst);
  ck_assert_int_eq(result, 0);
  ck_assert_float_eq(dst, 1e+07);
}
END_TEST

/// negative()

START_TEST(test_s21_negate_normal) {
  s21_decimal value = {{1, 0, 0, 0}};
  s21_decimal result;
  int flag = s21_negate(value, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_int_eq(get_sign(result), 1);
}
END_TEST

START_TEST(test_s21_negate_normal_negative) {
  s21_decimal value = {{1, 0, 0, 1 << 31}};
  s21_decimal result;
  int flag = s21_negate(value, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_int_eq(get_sign(result), 0);
}
END_TEST

START_TEST(test_s21_negate_zero_positive) {
  s21_decimal value = {{0, 0, 0, 0}};
  s21_decimal result;
  int flag = s21_negate(value, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_int_eq(get_sign(result), 1);
}
END_TEST

START_TEST(test_s21_negate_zero_negative) {
  s21_decimal value = {{0, 0, 0, 1 << 31}};
  s21_decimal result;
  int flag = s21_negate(value, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_int_eq(get_sign(result), 0);
}
END_TEST

START_TEST(test_s21_negate_null_result) {
  s21_decimal value = {{1, 0, 0, 0}};
  int flag = s21_negate(value, NULL);
  ck_assert_int_eq(flag, 1);
}
END_TEST

START_TEST(test_s21_negate_max_positive_value) {
  s21_decimal value = {{4294967295, 4294967295, 4294967295, 0}};
  s21_decimal result;
  int flag = s21_negate(value, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_int_eq(get_sign(result), 1);
}
END_TEST

START_TEST(test_s21_negate_max_negative_value) {
  s21_decimal value = {{4294967295, 4294967295, 4294967295, 1 << 31}};
  s21_decimal result;
  int flag = s21_negate(value, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_int_eq(get_sign(result), 0);
}
END_TEST

START_TEST(test_s21_negate_edge_case_positive) {
  s21_decimal value = {{12345, 0, 0, 0}};
  s21_decimal result;
  int flag = s21_negate(value, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_int_eq(get_sign(result), 1);
}
END_TEST

START_TEST(test_s21_negate_edge_case_negative) {
  s21_decimal value = {{12345, 0, 0, 1 << 31}};
  s21_decimal result;
  int flag = s21_negate(value, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_int_eq(get_sign(result), 0);
}
END_TEST

START_TEST(test_s21_negate_min_negative_value) {
  s21_decimal value = {{0, 0, 0, 1 << 31}};
  s21_decimal result;
  int flag = s21_negate(value, &result);
  ck_assert_int_eq(flag, 0);
  ck_assert_int_eq(get_sign(result), 0);
}
END_TEST

///// truncate

START_TEST(test_truncate_normal_values) {
  s21_decimal value = {{1234567890, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_truncate(value, &result), 0);
  ck_assert_int_eq(result.bits[0], 1234567890);
  ck_assert_int_eq(get_scale(result), 0);
}

START_TEST(test_truncate_edge_cases) {
  s21_decimal value = {{1234567890, 0, 0, 0}};
  s21_decimal result;

  set_scale(&value, 1);  // Устанавливаем масштаб 1
  ck_assert_int_eq(s21_truncate(value, &result), 0);
  ck_assert_int_eq(result.bits[0], 123456789);
  ck_assert_int_eq(get_scale(result), 0);
}

START_TEST(test_truncate_invalid_scale) {
  s21_decimal value = {{1, 0, 0, 0}};
  s21_decimal result;
  set_scale(&value, -1);  // Устанавливаем некорректный масштаб
  ck_assert_int_eq(s21_truncate(value, &result), 0);
  ck_assert_int_eq(result.bits[0], 0);
}

START_TEST(test_truncate_zero) {
  s21_decimal value = {{0, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_truncate(value, &result), 0);
  ck_assert_int_eq(result.bits[0], 0);
}

START_TEST(test_truncate_negative_value) {
  s21_decimal value = {{1, 0, 0, 0}};
  value.bits[3] = 0x80000000;
  s21_decimal result;
  ck_assert_int_eq(s21_truncate(value, &result), 0);
  ck_assert_int_eq(result.bits[0], 1);
}

START_TEST(test_truncate_scale_two) {
  s21_decimal value = {{123450000, 0, 0, 0}};
  s21_decimal result;
  set_scale(&value, 2);
  ck_assert_int_eq(s21_truncate(value, &result), 0);
  ck_assert_int_eq(result.bits[0], 1234500);
  ck_assert_int_eq(get_scale(result), 0);
}

START_TEST(add_0) {
  s21_decimal val1 = {{15, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST
START_TEST(add) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  s21_decimal val2 = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  set_scale(&val2, 28);
  s21_decimal res;
  ck_assert_int_eq(1, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_1) {
  s21_decimal val1 = {{15, 0, 0, 0}};
  s21_decimal val2 = {{15, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_2) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  s21_decimal val2 = {{1, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(1, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_3) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(2, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_4) {
  s21_decimal val1 = {{8, 0, 0, 0}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_5) {
  s21_decimal val1 = {{2, 0, 0, 0}};
  s21_decimal val2 = {{8, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_6) {
  s21_decimal val1 = {{8, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_7) {
  s21_decimal val1 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{8, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_8) {
  s21_decimal val1 = {{0}};
  s21_decimal val2 = {{0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_9) {
  s21_decimal val1 = {{4, 0, 0, 0}};
  s21_decimal val2 = {{8, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_10) {
  s21_decimal val1 = {{8, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_11) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{4, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_12) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{4, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(2, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_13) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  s21_decimal val2 = {{4, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_14) {
  s21_decimal val1 = {{4, 0, 0, 0}};
  s21_decimal val2 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_15) {
  s21_decimal val1 = {{4, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_16) {
  s21_decimal val1 = {{4, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(2, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_17) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  s21_decimal val2 = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  set_scale(&val1, 5);
  set_scale(&val2, 3);
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(add_18) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  set_scale(&val1, 5);
  set_scale(&val2, 3);
  s21_decimal res;
  ck_assert_int_eq(0, s21_add(val1, val2, &res));
}
END_TEST

START_TEST(mul_0) {
  s21_decimal val1 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(0, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(mul_1) {
  s21_decimal val1 = {{2, 0, 0, 0}};
  s21_decimal val2 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(0, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(mul_2) {
  s21_decimal val1 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(0, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(mul_3) {
  s21_decimal val1 = {{2, 0, 0, 0}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(0, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(mul_4) {
  s21_decimal val1 = {{8, 0, 0, 0}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(0, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(mul_5) {
  s21_decimal val1 = {{2, 0, 0, 0}};
  s21_decimal val2 = {{8, 0, 0, 0}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(0, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(mul_6) {
  s21_decimal val1 = {{8, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(0, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(mul_7) {
  s21_decimal val1 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{8, 0, 0, 0}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(0, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(mul_8) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(1, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(mul_9) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(2, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(mul_10) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(1, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(mul_11) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{0, 0, 0, 0}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(0, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(mul_12) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res = {{0}};
  set_scale(&val1, 1);
  ck_assert_int_eq(0, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(mul_13) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  s21_decimal val2 = {{200, 0, 0, 0}};
  s21_decimal res = {{0}};
  set_scale(&val2, 1);
  set_scale(&val2, 2);
  ck_assert_int_eq(0, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(mul_14) {
  s21_decimal val2 = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  s21_decimal val1 = {{20, 0, 0, 0}};
  s21_decimal res = {{0}};
  set_scale(&val2, 1);
  set_scale(&val2, 2);
  ck_assert_int_eq(0, s21_mul(val1, val2, &res));
}
END_TEST

START_TEST(div_0) {
  s21_decimal val1 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(0, s21_div(val1, val2, &res));
}
END_TEST

START_TEST(div_1) {
  s21_decimal val1 = {{2, 0, 0, 0}};
  s21_decimal val2 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(0, s21_div(val1, val2, &res));
}
END_TEST

START_TEST(div_2) {
  s21_decimal val1 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(0, s21_div(val1, val2, &res));
}
END_TEST

START_TEST(div_3) {
  s21_decimal val1 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{0, 0, 0, 0}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(3, s21_div(val1, val2, &res));
}
END_TEST

START_TEST(div_4) {
  s21_decimal val1 = {{2, 0, 0, 0}};
  s21_decimal val2 = {{0, 0, 0, 0}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(3, s21_div(val1, val2, &res));
}
END_TEST

START_TEST(div_5) {
  s21_decimal val1 = {{3, 0, 0, 0}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res = {{0}};
  ck_assert_int_eq(0, s21_div(val1, val2, &res));
}
END_TEST
START_TEST(negate_0) {
  s21_decimal val = {{2, 0, 0, 0}};
  s21_decimal res = {0};
  int sign_before = get_sign(val);
  s21_negate(val, &res);
  int sign_after = get_sign(res);

  ck_assert_int_ne(sign_before, sign_after);
}
END_TEST

START_TEST(negate_1) {
  s21_decimal val = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res = {0};
  int sign_before = get_sign(val);
  s21_negate(val, &res);
  int sign_after = get_sign(res);
  ck_assert_int_ne(sign_before, sign_after);
}
END_TEST

START_TEST(negate_2) {
  s21_decimal val = {{0, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res = {0};
  int sign_before = get_sign(val);
  s21_negate(val, &res);
  int sign_after = get_sign(res);
  ck_assert_int_ne(sign_before, sign_after);
}
END_TEST

START_TEST(negate_3) {
  s21_decimal val = {0};
  s21_decimal res = {0};
  int sign_before = get_sign(val);
  s21_negate(val, &res);
  int sign_after = 1;
  ck_assert_int_ne(sign_before, sign_after);
}
END_TEST

START_TEST(floor_0) {
  s21_decimal val = {{2, 0, 0, 0}};
  s21_decimal res = {0};
  ck_assert_int_eq(0, s21_floor(val, &res));
}
END_TEST

START_TEST(floor_1) {
  s21_decimal val = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res = {0};
  s21_floor(val, &res);
  float fres = 0;
  s21_from_decimal_to_float(res, &fres);
  ck_assert_float_eq(-2, fres);
}
END_TEST

START_TEST(floor_2) {
  s21_decimal val = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res = {0};
  set_scale(&val, 5);
  s21_floor(val, &res);
  float fres = 0;
  s21_from_decimal_to_float(res, &fres);
  ck_assert_float_eq(-1, fres);
}
END_TEST

START_TEST(floor_3) {
  s21_decimal val = {{2, 0, 0, 0}};
  s21_decimal res = {0};
  set_scale(&val, 5);
  s21_floor(val, &res);
  float fres = 0;
  s21_from_decimal_to_float(res, &fres);
  ck_assert_float_eq(0, fres);
}
END_TEST

START_TEST(round_0) {
  s21_decimal val = {{7, 7, 7, 0}};
  s21_decimal res = {0};
  s21_round(val, &res);
  int scale_val = get_scale(val);
  int scale_res = get_scale(res);
  ck_assert_int_eq(scale_val, scale_res);
}
END_TEST

START_TEST(round_1) {
  s21_decimal val = {{3, 3, 3, ~(UINT_MAX / 2)}};
  s21_decimal res = {0};
  ck_assert_int_eq(0, s21_round(val, &res));
}
END_TEST

START_TEST(round_2) {
  s21_decimal val = {{3, 3, 3, 0}};
  s21_decimal res = {0};
  set_scale(&val, 5);
  ck_assert_int_eq(0, s21_round(val, &res));
}
END_TEST

START_TEST(round_3) {
  s21_decimal val = {{7, 7, 7, ~(UINT_MAX / 2)}};
  s21_decimal res = {0};
  set_scale(&val, 5);
  ck_assert_int_eq(0, s21_round(val, &res));
}
END_TEST

START_TEST(round_4) {
  s21_decimal val = {{25, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res = {0};
  set_scale(&val, 1);
  ck_assert_int_eq(0, s21_round(val, &res));
}
END_TEST

START_TEST(round_5) {
  s21_decimal val = {{26, 0, 0, 0}};
  s21_decimal res = {0};
  set_scale(&val, 1);
  ck_assert_int_eq(0, s21_round(val, &res));
}
END_TEST

START_TEST(round_6) {
  s21_decimal val = {{115, 0, 0, 0}};
  s21_decimal res = {0};
  set_scale(&val, 1);
  ck_assert_int_eq(0, s21_round(val, &res));
}
END_TEST

START_TEST(round_7) {
  s21_decimal val = {{118, 0, 0, 0}};
  s21_decimal res = {0};
  set_scale(&val, 1);
  ck_assert_int_eq(0, s21_round(val, &res));
}
END_TEST

START_TEST(round_8) {
  s21_decimal val = {{125, 0, 0, 0}};
  s21_decimal res = {0};
  set_scale(&val, 1);
  ck_assert_int_eq(0, s21_round(val, &res));
}
END_TEST

START_TEST(round_9) {
  s21_decimal val = {{128, 0, 0, 0}};
  s21_decimal res = {0};
  set_scale(&val, 1);
  s21_round(val, &res);
  float fres = 0;
  s21_from_decimal_to_float(res, &fres);
  float need = 13;
  ck_assert_float_eq(need, fres);
}
END_TEST

START_TEST(from_decimal_to_float_0) {
  s21_decimal val = {{123456789, 0, 0, 0}};
  set_scale(&val, 5);
  float res = 1234.56789;
  float tmp = 0.;
  float *dst = &tmp;
  s21_from_decimal_to_float(val, dst);
  ck_assert_float_eq(*dst, res);
}
END_TEST

START_TEST(from_decimal_to_float_1) {
  s21_decimal val = {{123456789, 0, 0, 0}};
  set_sign(&val, 1);
  set_scale(&val, 5);
  float res = -1234.56789;
  float tmp = 0.;
  float *dst = &tmp;
  s21_from_decimal_to_float(val, dst);
  ck_assert_float_eq(*dst, res);
}
END_TEST

START_TEST(from_float_to_decimal_0) {
  s21_decimal val = {{0, 0, 0, 0}};
  float res = -1234.567871;
  float tmp = -1234.56789;
  s21_from_float_to_decimal(tmp, &val);
  ck_assert_float_eq(res, tmp);
}
END_TEST

START_TEST(from_float_to_decimal_1) {
  s21_decimal val = {{0, 0, 0, 0}};
  float res = 1234.567871;
  float tmp = 1234.56789;
  s21_from_float_to_decimal(tmp, &val);
  ck_assert_float_eq(res, tmp);
}
END_TEST

START_TEST(from_decimal_to_int_0) {
  s21_decimal val = {{123456789, 0, 0, 0}};
  set_scale(&val, 5);
  int res = 1234;
  int tmp = 0;
  int *dst = &tmp;
  s21_from_decimal_to_int(val, dst);
  ck_assert_int_eq(*dst, res);
}
END_TEST

START_TEST(from_decimal_to_int_1) {
  s21_decimal val = {{123456789, 0, 0, 0}};
  set_sign(&val, 1);
  set_scale(&val, 5);
  int res = -1234;
  int tmp = 0.;
  int *dst = &tmp;
  s21_from_decimal_to_int(val, dst);
  ck_assert_int_eq(*dst, res);
}
END_TEST

START_TEST(from_int_to_decimal_0) {
  s21_decimal val = {{0, 0, 0, 0}};
  int res = 0;
  int tmp = 123456789;
  s21_from_int_to_decimal(tmp, &val);
  s21_from_decimal_to_int(val, &res);
  ck_assert_int_eq(res, tmp);
}
END_TEST

START_TEST(from_int_to_decimal_1) {
  s21_decimal val = {{0, 0, 0, 0}};
  int res = 0;
  int tmp = -123456789;
  s21_from_int_to_decimal(tmp, &val);
  s21_from_decimal_to_int(val, &res);
  ck_assert_int_eq(res, tmp);
}
END_TEST

START_TEST(truncate_0) {
  s21_decimal val = {{7, 7, 7, 0}};
  s21_decimal res = {0};
  ck_assert_int_eq(0, s21_truncate(val, &res));
  int scale_val = get_scale(val);
  int scale_res = get_scale(res);
  ck_assert_int_eq(scale_val, scale_res);
  float fres = 0;
  s21_from_decimal_to_float(res, &fres);
  float need = 129127208515966861312.0;
  ck_assert_float_eq(need, fres);
}
END_TEST

START_TEST(truncate_1) {
  s21_decimal val = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res = {0};
  ck_assert_int_eq(0, s21_truncate(val, &res));
  float fres = 0;
  s21_from_decimal_to_float(res, &fres);
  float need = -2.0;
  ck_assert_float_eq(need, fres);
}
END_TEST

START_TEST(truncate_2) {
  s21_decimal val = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res = {0};
  ck_assert_int_eq(0, s21_truncate(val, &res));
  float fres = 0;
  s21_from_decimal_to_float(res, &fres);
  float need = -2.0;
  ck_assert_float_eq(need, fres);
}
END_TEST

START_TEST(truncate_3) {
  s21_decimal val = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res = {0};
  set_scale(&val, 5);
  ck_assert_int_eq(0, s21_truncate(val, &res));
  float fres = 0;
  s21_from_decimal_to_float(res, &fres);
  float need = -0.0;
  ck_assert_float_eq(need, fres);
}

START_TEST(truncate_4) {
  s21_decimal val = {{128, 0, 0, 0}};
  s21_decimal res = {0};
  set_scale(&val, 1);
  ck_assert_int_eq(0, s21_truncate(val, &res));
  float fres = 0;
  s21_from_decimal_to_float(res, &fres);
  float need = 12;
  ck_assert_float_eq(need, fres);
}

START_TEST(sub_0) {
  s21_decimal val1 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_1) {
  s21_decimal val1 = {{2, 0, 0, 0}};
  s21_decimal val2 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_2) {
  s21_decimal val1 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_3) {
  s21_decimal val1 = {{2, 0, 0, 0}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_4) {
  s21_decimal val1 = {{8, 0, 0, 0}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_5) {
  s21_decimal val1 = {{2, 0, 0, 0}};
  s21_decimal val2 = {{8, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_6) {
  s21_decimal val1 = {{8, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_7) {
  s21_decimal val1 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{8, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_8) {
  s21_decimal val1 = {{2, 0, 0, 0}};
  s21_decimal val2 = {{8, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_9) {
  s21_decimal val1 = {{8, 0, 0, 0}};
  s21_decimal val2 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_10) {
  s21_decimal val1 = {{8, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_11) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{4, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(2, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_12) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  s21_decimal val2 = {{4, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_13) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{4, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_14) {
  s21_decimal val1 = {{4, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  s21_decimal res;
  ck_assert_int_eq(2, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_15) {
  s21_decimal val1 = {{4, 0, 0, 0}};
  s21_decimal val2 = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_16) {
  s21_decimal val1 = {{4, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_17) {
  s21_decimal val1 = {{UINT_MAX, UINT_MAX, UINT_MAX, 0}};
  s21_decimal val2 = {{4, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(1, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_18) {
  s21_decimal val1 = {{4, 0, 0, 0}};
  s21_decimal val2 = {{UINT_MAX, UINT_MAX, UINT_MAX, ~(UINT_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(1, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_19) {
  s21_decimal val1 = {{4, 0, 0, 0}};
  s21_decimal val2 = {{4, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(0, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_20) {
  s21_decimal dec1;
  s21_decimal dec2;
  int tmp1 = 100;
  int tmp2 = 99999;
  int res_s21 = 0;
  int res = -99899;
  s21_decimal res1;
  s21_from_int_to_decimal(tmp1, &dec1);
  s21_from_int_to_decimal(tmp2, &dec2);
  s21_sub(dec1, dec2, &res1);
  s21_from_decimal_to_int(res1, &res_s21);
  ck_assert_int_eq(res_s21, res);
}
END_TEST

START_TEST(sub_21) {
  s21_decimal dec1;
  s21_decimal dec2;
  int tmp1 = -100;
  int tmp2 = -99999;
  int res_s21 = 0;
  s21_decimal res1;
  int res = 99899;
  s21_from_int_to_decimal(tmp1, &dec1);
  s21_from_int_to_decimal(tmp2, &dec2);
  s21_sub(dec1, dec2, &res1);
  s21_from_decimal_to_int(res1, &res_s21);
  ck_assert_int_eq(res_s21, res);
}
END_TEST

START_TEST(test_eq_0) {
  s21_decimal val1 = {{2, 0, 0, 0}};
  s21_decimal val2 = {{2, 0, 0, ~(UINT_MAX / 2)}};

  ck_assert_int_eq(0, s21_is_equal(val1, val2));
}
END_TEST

START_TEST(test_eq_1) {
  s21_decimal val1 = {{2, 0, 0, ~(UINT_MAX / 2)}};
  s21_decimal val2 = {{2, 0, 0, ~(UINT_MAX / 2)}};

  ck_assert_int_eq(1, s21_is_equal(val1, val2));
}

START_TEST(test_eq_2) {
  s21_decimal val1 = {{2, 2, 2, 0}};
  set_scale(&val1, 5);
  s21_decimal val2 = {{2, 2, 2, 0}};

  ck_assert_int_eq(0, s21_is_equal(val1, val2));
}
END_TEST

START_TEST(test_eq_3) {
  s21_decimal val1 = {{0, 0, 0, 0}};
  set_sign(&val1, 1);
  s21_decimal val2 = {{0, 0, 0, 0}};

  ck_assert_int_eq(1, s21_is_equal(val1, val2));
}
END_TEST

START_TEST(test_less_0) {
  s21_decimal val1 = {{2, 2, 2, 0}};
  set_scale(&val1, 5);
  s21_decimal val2 = {{2, 2, 2, 0}};

  ck_assert_int_eq(1, s21_is_less(val1, val2));
}
END_TEST

START_TEST(test_less_1) {
  s21_decimal val1 = {{0, 0, 0, 0}};

  s21_decimal val2 = {{0, 0, 0, 0}};

  ck_assert_int_eq(0, s21_is_less(val1, val2));
}
END_TEST

START_TEST(test_less_2) {
  s21_decimal val1 = {{0, 5, 0, 0}};
  set_sign(&val1, 1);
  s21_decimal val2 = {{0, 0, 0, 0}};

  ck_assert_int_eq(1, s21_is_less(val1, val2));
}
END_TEST

START_TEST(test_less_3) {
  s21_decimal val1 = {{123, 0, 0, 0}};
  set_sign(&val1, 1);
  s21_decimal val2 = {{5, 0, 0, 0}};
  set_sign(&val2, 1);
  ck_assert_int_eq(1, s21_is_less(val1, val2));
}
END_TEST

START_TEST(test_not_eq_0) {
  s21_decimal val1 = {{2, 2, 2, 0}};
  set_scale(&val1, 5);
  s21_decimal val2 = {{2, 2, 2, 0}};

  ck_assert_int_eq(1, s21_is_not_equal(val1, val2));
}
END_TEST

START_TEST(test_not_eq_1) {
  s21_decimal val1 = {{2, 2, 2, 0}};

  s21_decimal val2 = {{2, 2, 2, 0}};

  ck_assert_int_eq(0, s21_is_not_equal(val1, val2));
}
END_TEST

START_TEST(test_less_or_eq_0) {
  s21_decimal val1 = {{2, 2, 2, 0}};

  s21_decimal val2 = {{2, 2, 2, 0}};

  ck_assert_int_eq(1, s21_is_less_or_equal(val1, val2));
}
END_TEST

START_TEST(test_less_or_eq_1) {
  s21_decimal val1 = {{2, 2, 2, 0}};
  set_sign(&val1, 1);
  s21_decimal val2 = {{2, 2, 2, 0}};

  ck_assert_int_eq(1, s21_is_less_or_equal(val1, val2));
}
END_TEST

START_TEST(test_less_or_eq_2) {
  s21_decimal val1 = {{2, 2, 2, 0}};
  set_sign(&val1, 1);
  s21_decimal val2 = {{2, 2, 2, 0}};
  set_scale(&val2, 5);
  ck_assert_int_eq(1, s21_is_less_or_equal(val1, val2));
}
END_TEST

START_TEST(test_less_or_eq_3) {
  s21_decimal val1 = {{2, 2, 2, 0}};

  s21_decimal val2 = {{2, 2, 2, 0}};
  set_scale(&val2, 5);
  ck_assert_int_eq(0, s21_is_less_or_equal(val1, val2));
}
END_TEST

START_TEST(test_greater_or_eq_0) {
  s21_decimal val1 = {{2, 2, 2, 0}};

  s21_decimal val2 = {{2, 2, 2, 0}};

  ck_assert_int_eq(1, s21_is_greater_or_equal(val1, val2));
}
END_TEST

START_TEST(test_greater_or_eq_1) {
  s21_decimal val1 = {{2, 2, 2, 0}};
  set_sign(&val1, 1);
  s21_decimal val2 = {{2, 2, 2, 0}};

  ck_assert_int_eq(0, s21_is_greater_or_equal(val1, val2));
}
END_TEST

START_TEST(test_greater_or_eq_2) {
  s21_decimal val1 = {{2, 2, 2, 0}};
  set_sign(&val1, 1);
  s21_decimal val2 = {{2, 2, 2, 0}};
  set_scale(&val2, 5);
  ck_assert_int_eq(0, s21_is_greater_or_equal(val1, val2));
}
END_TEST

START_TEST(test_greater_or_eq_3) {
  s21_decimal val1 = {{2, 2, 2, 0}};

  s21_decimal val2 = {{2, 2, 2, 0}};
  set_scale(&val2, 5);
  ck_assert_int_eq(1, s21_is_greater_or_equal(val1, val2));
}
END_TEST

START_TEST(test_s21_is_greater_0) {
  s21_decimal val1 = {{2, 2, 2, 0}};
  set_scale(&val1, 5);
  s21_decimal val2 = {{2, 2, 2, 0}};

  ck_assert_int_eq(0, s21_is_greater(val1, val2));
}
END_TEST

START_TEST(test_s21_is_greater_1) {
  s21_decimal val1 = {{0, 0, 0, 0}};

  s21_decimal val2 = {{0, 0, 0, 0}};

  ck_assert_int_eq(0, s21_is_greater(val1, val2));
}
END_TEST

START_TEST(test_s21_is_greater_2) {
  s21_decimal val1 = {{0, 5, 0, 0}};
  set_sign(&val1, 1);
  s21_decimal val2 = {{0, 0, 0, 0}};

  ck_assert_int_eq(0, s21_is_greater(val1, val2));
}
END_TEST

START_TEST(test_s21_is_greater_3) {
  s21_decimal val1 = {{5, 0, 0, 0}};
  set_sign(&val1, 1);
  s21_decimal val2 = {{123, 0, 0, 0}};
  set_sign(&val2, 1);
  ck_assert_int_eq(1, s21_is_greater(val1, val2));
}
END_TEST

Suite *s21_decimal_suite(void) {
  Suite *s;
  TCase *tc_core;
  s = suite_create("s21_decimal");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_s21_from_int_to_decimal_positive);
  tcase_add_test(tc_core, test_s21_from_int_to_decimal_negative);
  tcase_add_test(tc_core, test_s21_from_int_to_decimal_zero);
  tcase_add_test(tc_core, test_s21_from_int_to_decimal_max_int);
  tcase_add_test(tc_core, test_s21_from_int_to_decimal_min_int);
  tcase_add_test(tc_core, test_s21_from_int_to_decimal_overflow_positive);
  tcase_add_test(tc_core, test_s21_from_int_to_decimal_overflow_negative);
  tcase_add_test(tc_core, test_s21_from_int_to_decimal_invalid_pointer);

  tcase_add_test(tc_core, test_normal_values);
  tcase_add_test(tc_core, test_negative_value);
  tcase_add_test(tc_core, test_scaling);
  tcase_add_test(tc_core, test_large_scale);
  tcase_add_test(tc_core, test_null_pointer);

  tcase_add_test(tc_core, test_null_dst);
  tcase_add_test(tc_core, test_small_float);
  tcase_add_test(tc_core, test_large_float);
  tcase_add_test(tc_core, test_infinity);
  tcase_add_test(tc_core, test_negative_float);
  tcase_add_test(tc_core, test_positive_float);
  tcase_add_test(tc_core, test_small_positive);
  tcase_add_test(tc_core, test_small_negative);
  tcase_add_test(tc_core, test_high_precision);
  tcase_add_test(tc_core, test_normal_value);

  tcase_add_test(tc_core, test_dec_to_float_zero);
  tcase_add_test(tc_core, test_dec_to_float_small_positive);
  tcase_add_test(tc_core, test_dec_to_float_small_negative);
  tcase_add_test(tc_core, test_dec_to_float_large_positive);
  tcase_add_test(tc_core, test_dec_to_float_large_negative);
  tcase_add_test(tc_core, test_dec_to_float_scale_reduction);
  tcase_add_test(tc_core, test_dec_to_float_overflow);
  tcase_add_test(tc_core, test_dec_to_float_underflow);
  tcase_add_test(tc_core, test_dec_to_float_exact_fit);

  tcase_add_test(tc_core, test_s21_negate_normal);
  tcase_add_test(tc_core, test_s21_negate_normal_negative);
  tcase_add_test(tc_core, test_s21_negate_zero_positive);
  tcase_add_test(tc_core, test_s21_negate_zero_negative);
  tcase_add_test(tc_core, test_s21_negate_null_result);
  tcase_add_test(tc_core, test_s21_negate_max_positive_value);
  tcase_add_test(tc_core, test_s21_negate_max_negative_value);
  tcase_add_test(tc_core, test_s21_negate_edge_case_positive);
  tcase_add_test(tc_core, test_s21_negate_edge_case_negative);
  tcase_add_test(tc_core, test_s21_negate_min_negative_value);

  tcase_add_test(tc_core, test_truncate_normal_values);
  tcase_add_test(tc_core, test_truncate_edge_cases);
  tcase_add_test(tc_core, test_truncate_invalid_scale);
  tcase_add_test(tc_core, test_truncate_zero);
  tcase_add_test(tc_core, test_truncate_negative_value);
  tcase_add_test(tc_core, test_truncate_scale_two);

  tcase_add_test(tc_core, add);
  tcase_add_test(tc_core, add_0);  // s21_add
  tcase_add_test(tc_core, add_1);
  tcase_add_test(tc_core, add_2);
  tcase_add_test(tc_core, add_3);
  tcase_add_test(tc_core, add_4);
  tcase_add_test(tc_core, add_5);
  tcase_add_test(tc_core, add_6);
  tcase_add_test(tc_core, add_7);
  tcase_add_test(tc_core, add_8);
  tcase_add_test(tc_core, add_9);
  tcase_add_test(tc_core, add_10);
  tcase_add_test(tc_core, add_11);
  tcase_add_test(tc_core, add_12);
  tcase_add_test(tc_core, add_13);
  tcase_add_test(tc_core, add_14);
  tcase_add_test(tc_core, add_15);
  tcase_add_test(tc_core, add_16);
  tcase_add_test(tc_core, add_17);
  tcase_add_test(tc_core, add_18);

  tcase_add_test(tc_core, mul_0);  // s21_mul
  tcase_add_test(tc_core, mul_1);
  tcase_add_test(tc_core, mul_2);
  tcase_add_test(tc_core, mul_3);
  tcase_add_test(tc_core, mul_4);
  tcase_add_test(tc_core, mul_5);
  tcase_add_test(tc_core, mul_6);
  tcase_add_test(tc_core, mul_7);
  tcase_add_test(tc_core, mul_8);
  tcase_add_test(tc_core, mul_9);
  tcase_add_test(tc_core, mul_10);
  tcase_add_test(tc_core, mul_11);
  tcase_add_test(tc_core, mul_12);
  tcase_add_test(tc_core, mul_13);
  tcase_add_test(tc_core, mul_14);
  tcase_add_test(tc_core, div_0);  // s21_div
  tcase_add_test(tc_core, div_1);
  tcase_add_test(tc_core, div_2);
  tcase_add_test(tc_core, div_3);
  tcase_add_test(tc_core, div_4);
  tcase_add_test(tc_core, div_5);
  tcase_add_test(tc_core, negate_0);  // negate
  tcase_add_test(tc_core, negate_1);
  tcase_add_test(tc_core, negate_2);
  tcase_add_test(tc_core, negate_3);
  tcase_add_test(tc_core, round_0);  // round
  tcase_add_test(tc_core, round_1);
  tcase_add_test(tc_core, round_2);
  tcase_add_test(tc_core, round_3);
  tcase_add_test(tc_core, round_4);
  tcase_add_test(tc_core, round_5);
  tcase_add_test(tc_core, round_6);
  tcase_add_test(tc_core, round_7);
  tcase_add_test(tc_core, round_8);
  tcase_add_test(tc_core, round_9);
  tcase_add_test(tc_core, truncate_0);  // truncate
  tcase_add_test(tc_core, truncate_1);
  tcase_add_test(tc_core, truncate_2);
  tcase_add_test(tc_core, truncate_3);
  tcase_add_test(tc_core, truncate_4);
  tcase_add_test(tc_core, floor_0);  // floor
  tcase_add_test(tc_core, floor_1);
  tcase_add_test(tc_core, floor_2);
  tcase_add_test(tc_core, floor_3);
  tcase_add_test(tc_core, from_decimal_to_float_0);  // from_decimal_to_float
  tcase_add_test(tc_core, from_decimal_to_float_1);
  tcase_add_test(tc_core, from_float_to_decimal_0);  // from_float_to_decimal
  tcase_add_test(tc_core, from_float_to_decimal_1);
  tcase_add_test(tc_core, from_decimal_to_int_0);  // from_decimal_to_int
  tcase_add_test(tc_core, from_decimal_to_int_1);
  tcase_add_test(tc_core, from_int_to_decimal_0);  // from_int_to_decimal
  tcase_add_test(tc_core, from_int_to_decimal_1);

  tcase_add_test(tc_core, sub_0);  // s21_sub
  tcase_add_test(tc_core, sub_1);
  tcase_add_test(tc_core, sub_2);
  tcase_add_test(tc_core, sub_3);
  tcase_add_test(tc_core, sub_4);
  tcase_add_test(tc_core, sub_5);
  tcase_add_test(tc_core, sub_6);
  tcase_add_test(tc_core, sub_7);
  tcase_add_test(tc_core, sub_8);
  tcase_add_test(tc_core, sub_9);
  tcase_add_test(tc_core, sub_10);
  tcase_add_test(tc_core, sub_11);
  tcase_add_test(tc_core, sub_12);
  tcase_add_test(tc_core, sub_13);
  tcase_add_test(tc_core, sub_14);
  tcase_add_test(tc_core, sub_15);
  tcase_add_test(tc_core, sub_16);
  tcase_add_test(tc_core, sub_17);
  tcase_add_test(tc_core, sub_18);
  tcase_add_test(tc_core, sub_19);
  tcase_add_test(tc_core, sub_20);
  tcase_add_test(tc_core, sub_21);
  tcase_add_test(tc_core, test_eq_0);
  tcase_add_test(tc_core, test_eq_1);
  tcase_add_test(tc_core, test_eq_2);
  tcase_add_test(tc_core, test_eq_3);
  tcase_add_test(tc_core, test_less_0);
  tcase_add_test(tc_core, test_less_1);
  tcase_add_test(tc_core, test_less_2);
  tcase_add_test(tc_core, test_less_3);
  tcase_add_test(tc_core, test_not_eq_0);
  tcase_add_test(tc_core, test_not_eq_1);
  tcase_add_test(tc_core, test_less_or_eq_0);
  tcase_add_test(tc_core, test_less_or_eq_1);
  tcase_add_test(tc_core, test_less_or_eq_2);
  tcase_add_test(tc_core, test_less_or_eq_3);
  tcase_add_test(tc_core, test_greater_or_eq_0);
  tcase_add_test(tc_core, test_greater_or_eq_1);
  tcase_add_test(tc_core, test_greater_or_eq_2);
  tcase_add_test(tc_core, test_greater_or_eq_3);
  tcase_add_test(tc_core, test_s21_is_greater_0);
  tcase_add_test(tc_core, test_s21_is_greater_1);
  tcase_add_test(tc_core, test_s21_is_greater_2);
  tcase_add_test(tc_core, test_s21_is_greater_3);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = s21_decimal_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? 0 : 1;
}
