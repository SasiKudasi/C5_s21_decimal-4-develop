#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned int bits[4];
} s21_decimal;

typedef struct {
  unsigned int bits[6];
  int scale;
} big_decimal;

typedef union {
  float f;
  uint32_t i;
} float_to_int_union;

typedef union {
  int ui;
  float fl;
} floatbits;

void set_bit_int(unsigned int *value, int index);
int get_bit_int(int value, int index);
void turn_off_bit_int(unsigned int *value, int index);

void set_bit(big_decimal *value, int index);
int get_bit(big_decimal value, int index);
void turn_off_bit(big_decimal *value, int index);
int bit_addition(int first_value_bit, int second_value_bit, int *carry);
void invert_bits(big_decimal *value);
void twos_complement(big_decimal *value);
void set_sign(s21_decimal *value, int sign);
int get_sign(s21_decimal value);
void set_scale(s21_decimal *value, int scale);
int get_scale(s21_decimal value);
int compare(big_decimal first_value, big_decimal second_value);
void shift_left(big_decimal *value, int shift);
int get_bits_count(big_decimal value);

void shift_left_division(big_decimal *value, int shift);
s21_decimal s21_decimal_get_zero(void);
int float_sign(int *i, char my_float[20]);
int get_num(int *i, int *float_pow, char my_float[20]);
int get_pow(int *i, int *pow_sign, int *float_pow, char my_float[20]);
int s21_del_last_digit(s21_decimal *src);
int converter(int num, int float_pow, int pow_sign, s21_decimal *dst,
              float src);

void binary_add(big_decimal first_value, big_decimal second_value,
                big_decimal *result);
void binary_sub(big_decimal first_value, big_decimal second_value,
                big_decimal *result);
void binary_mul(big_decimal first_value, big_decimal second_value,
                big_decimal *result);
void binary_div(big_decimal val1, big_decimal val2, big_decimal *result);
void helper_for_binary_div(int shift, big_decimal *val1, big_decimal val2,
                           big_decimal *result, big_decimal tmp_val2,
                           big_decimal temp_result);

void norm_to_lower(int scale1, int scale2, big_decimal *val);
void norm_to_upper(int scale2, int scale1, big_decimal *val1);
void normalize_to_common_scale(s21_decimal val1, s21_decimal val2,
                               big_decimal *big_val1, big_decimal *big_val2);

big_decimal decimal_to_big(s21_decimal from);
void convert_to_decimal(big_decimal from, s21_decimal *to);
int big_decimal_round(big_decimal val1, big_decimal val2, s21_decimal *result);
int owerflow(big_decimal val1);
int big_decimal_to_decimal(big_decimal from, s21_decimal *to);

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);
int s21_floor(s21_decimal value, s21_decimal *result);

int s21_is_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_less(s21_decimal value_1, s21_decimal value_2);
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);

#endif
