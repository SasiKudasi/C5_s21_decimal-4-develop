#include "s21_decimal.h"
#define S21_UMAX_INT 0xFFFFFFFF

void turn_off_bit(big_decimal *value, int index) {
  int bit_index = index / 32;
  int bit_position = index % 32;
  value->bits[bit_index] &= ~(1 << bit_position);
}

void binary_add(big_decimal first_value, big_decimal second_value,
                big_decimal *result) {
  *result = (big_decimal){{0, 0, 0, 0, 0, 0}, 0};
  int carry = 0;
  int first_value_bit = 0;
  int second_value_bit = 0;
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 32; j++) {
      first_value_bit = get_bit_int(first_value.bits[i], j);
      second_value_bit = get_bit_int(second_value.bits[i], j);

      if (bit_addition(first_value_bit, second_value_bit, &carry))
        set_bit_int(&result->bits[i], j);
      else
        turn_off_bit_int(&result->bits[i], j);
    }
  }
}

int get_bit(big_decimal value, int index) {
  int bit_index = index / 32;

  int bit_position = index % 32;

  return (value.bits[bit_index] & (1 << bit_position)) != 0;
}

int bit_addition(int first_value_bit, int second_value_bit, int *carry) {
  int firstXOR = first_value_bit ^ second_value_bit;
  int secondXOR = firstXOR ^ *carry;
  int firstAND = *carry & firstXOR;
  int secondAND = first_value_bit & second_value_bit;
  *carry = firstAND | secondAND;
  return secondXOR;
}

void set_bit_int(unsigned int *value, int index) { *value |= (1 << index); }

int get_bit_int(int value, int index) { return (value & (1 << index)) != 0; }

void turn_off_bit_int(unsigned int *value, int index) {
  *value &= ~(1 << index);
}

void set_bit(big_decimal *value, int index) {
  int bit_index = index / 32;
  int bit_position = index % 32;
  value->bits[bit_index] |= (1 << bit_position);
}

void invert_bits(big_decimal *value) {
  for (int i = 0; i < 6; i++) {
    value->bits[i] = ~value->bits[i];
  }
}

void twos_complement(big_decimal *value) {
  invert_bits(value);

  big_decimal one = {{1, 0, 0, 0, 0, 0}, 0};
  binary_add(*value, one, value);
}

void binary_sub(big_decimal first_value, big_decimal second_value,
                big_decimal *result) {
  *result = (big_decimal){{0, 0, 0, 0, 0, 0}, 0};
  big_decimal twos_complement_second_value = {{0, 0, 0, 0, 0, 0}, 0};
  int flag = compare(first_value, second_value);
  if (flag) {
    if (flag == 1)
      twos_complement_second_value = second_value;

    else if (flag == 2)
      twos_complement_second_value = first_value;

    twos_complement(&twos_complement_second_value);

    if (flag == 1)
      binary_add(first_value, twos_complement_second_value, result);

    if (flag == 2)
      binary_add(second_value, twos_complement_second_value, result);
  }
}

void set_scale(s21_decimal *value, int scale) {
  value->bits[3] |= (scale << 16);

  for (int i = 0; i <= 15; i++) {
    value->bits[3] |= (0 << i);
  }

  for (int i = 24; i < 30; i++) {
    value->bits[3] |= (0 << i);
  }
}

int get_scale(s21_decimal value) {
  value.bits[3] &= ~(1 << 31);
  int res = value.bits[3] >> 16;
  return res;
}

void set_sign(s21_decimal *value, int sign) { value->bits[3] |= (sign << 31); }

int get_sign(s21_decimal value) {
  int res = (value.bits[3] & (1 << 31)) != 0;
  return res;
}

int compare(big_decimal first_value, big_decimal second_value) {
  int bit_one = 0;
  int bit_second = 0;
  int flag = 0;
  for (int i = 191; i >= 0 && !flag; i--) {
    bit_one = get_bit(first_value, i);
    bit_second = get_bit(second_value, i);
    if (bit_one == bit_second) continue;
    if (bit_one > bit_second) {
      flag = 1;
    } else if (bit_one < bit_second) {
      flag = 2;
    }
  }
  return flag;
}

void shift_left(big_decimal *value, int shift) {
  for (int i = 0; i < shift; i++) {
    unsigned int carry = 0;

    for (int j = 0; j < 6; j++) {
      unsigned int new_carry = (value->bits[j] & (1 << 31)) != 0;
      value->bits[j] = (value->bits[j] << 1) | carry;
      carry = new_carry;
    }
  }
}

void binary_mul(big_decimal first_value, big_decimal second_value,
                big_decimal *result) {
  *result = (big_decimal){{0, 0, 0, 0, 0, 0}, 0};
  big_decimal temp_result = {{0, 0, 0, 0, 0, 0}, 0};
  big_decimal multiplicand = first_value;
  int bits = get_bits_count(second_value);

  for (int i = 0; i <= bits; i++) {
    if (get_bit(second_value, i)) {
      shift_left(&multiplicand, i);
      binary_add(temp_result, multiplicand, &temp_result);
      multiplicand = first_value;
    }
  }
  *result = temp_result;
}

void binary_div(big_decimal val1, big_decimal val2, big_decimal *result) {
  big_decimal temp_result = {{0, 0, 0, 0, 0, 0}, 0};
  big_decimal temp_remainder = {{0, 0, 0, 0, 0, 0}, 0};
  big_decimal tmp_val2 = val2;
  int bit_count_val1 = get_bits_count(val1);
  int bit_count_val2 = get_bits_count(tmp_val2);
  int shift = bit_count_val1 - bit_count_val2;

  *result = (big_decimal){{0, 0, 0, 0, 0, 0}, 0};

  helper_for_binary_div(shift, &val1, val2, result, tmp_val2, temp_result);

  temp_remainder = val1;
  int scale = 0;
  big_decimal temp_fraction = {{0, 0, 0, 0, 0, 0}, 0};

  while (compare(temp_remainder, (big_decimal){{0, 0, 0, 0, 0, 0}, 0}) == 1 &&
         scale < 32) {
    scale++;

    binary_mul(temp_remainder, (big_decimal){{10, 0, 0, 0, 0, 0}, 0},
               &temp_remainder);

    shift = get_bits_count(temp_remainder) - get_bits_count(val2);

    helper_for_binary_div(shift, &temp_remainder, val2, &temp_fraction,
                          tmp_val2, temp_result);
    binary_mul(*result, (big_decimal){{10, 0, 0, 0, 0, 0}, 0}, result);
    binary_add(*result, temp_fraction, result);
  }
  result->scale = scale;
}

void helper_for_binary_div(int shift, big_decimal *val1, big_decimal val2,
                           big_decimal *result, big_decimal tmp_val2,
                           big_decimal temp_result) {
  *result = (big_decimal){{0, 0, 0, 0, 0, 0}, 0};
  big_decimal test = tmp_val2;
  while (shift >= 0) {
    shift_left_division(&tmp_val2, shift);
    if (compare(tmp_val2, *val1) == 1) {
      tmp_val2 = test;
      shift--;
      continue;
    }
    if (compare(*val1, tmp_val2) == 1 || compare(*val1, tmp_val2) == 0) {
      binary_sub(*val1, tmp_val2, &temp_result);
      *val1 = temp_result;
      set_bit(result, shift);
    } else
      turn_off_bit(result, shift);
    shift--;
    tmp_val2 = val2;
  }
}

void shift_left_division(big_decimal *value, int shift) {
  while (shift--) {
    unsigned int carry = 0;
    for (int i = 0; i < 6; i++) {
      unsigned int new_carry = value->bits[i] >> 31;
      value->bits[i] = (value->bits[i] << 1) | carry;
      carry = new_carry;
    }
  }
}

int get_bits_count(big_decimal value) {
  for (int i = 191; i >= 0; i--) {
    if (get_bit(value, i)) return i + 1;
  }
  return 0;
}

void normalize_to_common_scale(s21_decimal val1, s21_decimal val2,
                               big_decimal *big_val1, big_decimal *big_val2) {
  *big_val1 = decimal_to_big(val1);
  *big_val2 = decimal_to_big(val2);

  int scale1 = get_scale(val1);
  int scale2 = get_scale(val2);
  big_val1->scale = scale1;
  big_val2->scale = scale2;
  if (scale1 > 28 || scale2 > 28) {
    if (scale1 > 28 && scale2 > 28) {
      norm_to_lower(scale1, 28, big_val1);
      norm_to_lower(scale2, 28, big_val2);
    } else if (scale1 > 28 && scale2 <= 28) {
      norm_to_upper(28, scale2, big_val2);
      norm_to_lower(scale1, 28, big_val1);
    } else if (scale2 > 28 && scale1 <= 28) {
      norm_to_lower(scale2, 28, big_val2);
      norm_to_upper(28, scale1, big_val1);
    }
  } else {
    if (scale1 < scale2) {
      norm_to_upper(scale2, scale1, big_val1);
      big_val2->scale = big_val1->scale;
    } else if (scale1 > scale2) {
      norm_to_upper(scale1, scale2, big_val2);
    }
  }
}

void norm_to_upper(int scale2, int scale1, big_decimal *val1) {
  int difference_scale = scale2 - scale1;
  while (difference_scale > 0) {
    binary_mul(*val1, (big_decimal){{10, 0, 0, 0, 0, 0}, 0}, val1);
    difference_scale--;
  }

  val1->scale = scale2;
}

void norm_to_lower(int scale1, int scale2, big_decimal *val) {
  int difference_scale = scale1 - scale2;
  while (difference_scale > 0) {
    binary_div(*val, (big_decimal){{10, 0, 0, 0, 0, 0}, 0}, val);
    difference_scale--;
  }
  val->scale = scale2;
}

int big_decimal_to_decimal(big_decimal from, s21_decimal *to) {
  int flag = 0;
  if (owerflow(from) || from.scale > 28)
    flag = big_decimal_round(from, (big_decimal){{10, 0, 0, 0, 0, 0}, 0}, to);
  else {
    convert_to_decimal(from, to);
    set_scale(to, from.scale);
  }

  return flag;
}

void convert_to_decimal(big_decimal from, s21_decimal *to) {
  for (int i = 0; i < 3; i++) to->bits[i] = from.bits[i];
}

int big_decimal_round(big_decimal val1, big_decimal val2, s21_decimal *to) {
  int scale = val1.scale;
  int flag = 0;
  int flag_for_round = 0;
  if (scale > 0) {
    big_decimal temp_result = {{0, 0, 0, 0, 0, 0}, 0};
    big_decimal tmp_val2 = val2;
    big_decimal result = {{0, 0, 0, 0, 0, 0}, 0};
    big_decimal tmp_val1 = {{0, 0, 0, 0, 0, 0}, 0};

    int bit_count_val1 = get_bits_count(val1);
    int bit_count_val2 = get_bits_count(tmp_val2);
    int shift = bit_count_val1 - bit_count_val2;

    while ((((owerflow(val1) == 1 || owerflow(result) == 1) && scale > 0) ||
            scale > 28)) {
      helper_for_binary_div(shift, &val1, val2, &result, tmp_val2, temp_result);
      if (compare(val1, (big_decimal){{0, 0, 0, 0, 0, 0}, 0}) == 1 &&
          owerflow(result)) {
        flag_for_round = 1;
      }
      tmp_val1 = val1;

      val1 = result;
      scale--;
      bit_count_val1 = get_bits_count(val1);
      shift = bit_count_val1 - bit_count_val2;
    }
    int cmp = compare(tmp_val1, (big_decimal){{5, 0, 0, 0, 0, 0}, 0});
    if (!cmp && !flag_for_round && get_bit(result, 0)) {
      binary_add(result, (big_decimal){{1, 0, 0, 0, 0, 0}, 0}, &result);
    } else if (cmp == 1) {
      binary_add(result, (big_decimal){{1, 0, 0, 0, 0, 0}, 0}, &result);
    } else if (!cmp && flag_for_round) {
      binary_add(result, (big_decimal){{1, 0, 0, 0, 0, 0}, 0}, &result);
    }

    if (!owerflow(result)) {
      convert_to_decimal(result, to);
      set_scale(to, scale);
    } else
      flag = 1;
  } else
    flag = 1;

  return flag;
}

int owerflow(big_decimal val1) {
  int flag = 0;
  if (val1.bits[3] > 0 || val1.bits[4] > 0 || val1.bits[5] > 0) {
    flag = 1;
  }
  return flag;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag = 0;
  int sign_1 = get_sign(value_1);
  int sign_2 = get_sign(value_2);
  *result = (s21_decimal){{0, 0, 0, 0}};
  big_decimal big_val1 = {{0, 0, 0, 0, 0, 0}, 0},
              big_val2 = {{0, 0, 0, 0, 0, 0}, 0};
  big_decimal big_result = {{0, 0, 0, 0, 0, 0}, 0};
  normalize_to_common_scale(value_1, value_2, &big_val1, &big_val2);
  int cmp = compare(big_val1, big_val2);
  if (sign_1 != sign_2) {
    if (cmp == 1 && sign_1) {
      binary_sub(big_val1, big_val2, &big_result);
      set_sign(result, 1);
    }
    if (cmp == 2 && sign_2) {
      binary_sub(big_val2, big_val1, &big_result);
      set_sign(result, 1);
    }
    if (cmp == 1 && !sign_1) {
      binary_sub(big_val1, big_val2, &big_result);
      set_sign(result, 0);
    }
    if (cmp == 2 && !sign_2) {
      binary_sub(big_val1, big_val2, &big_result);
      set_sign(result, 0);
    }
  } else {
    if (sign_1 && sign_2) {
      binary_add(big_val1, big_val2, &big_result);
      set_sign(result, 1);
    } else {
      binary_add(big_val1, big_val2, &big_result);
      set_sign(result, 0);
    }
  }
  big_result.scale = big_val1.scale;
  flag = big_decimal_to_decimal(big_result, result);
  if (flag == 1 && get_sign(*result) == 1) {
    flag = 2;
  }
  if (flag == 1 && get_sign(*result) == 0) {
    flag = 1;
  }
  return flag;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  *result = (s21_decimal){{0, 0, 0, 0}};
  int flag = 0;
  int sign_1 = get_sign(value_1);
  int sign_2 = get_sign(value_2);
  big_decimal big_val1 = {{0, 0, 0, 0, 0, 0}, 0},
              big_val2 = {{0, 0, 0, 0, 0, 0}, 0};
  big_decimal big_result = {{0, 0, 0, 0, 0, 0}, 0};
  normalize_to_common_scale(value_1, value_2, &big_val1, &big_val2);

  int cmp = compare(big_val1, big_val2);
  if (cmp == 0 && sign_1) {
    set_sign(result, 1);
  }
  if (sign_1 != sign_2) {
    if (cmp == 1 && sign_1) {
      binary_add(big_val1, big_val2, &big_result);
      set_sign(result, 1);
    }
    if (cmp == 2 && sign_2) {
      binary_add(big_val2, big_val1, &big_result);
      set_sign(result, 0);
    }
    if (cmp == 1 && !sign_1) {
      binary_add(big_val1, big_val2, &big_result);
      set_sign(result, 0);
    }
    if (cmp == 2 && !sign_2) {
      binary_add(big_val1, big_val2, &big_result);
      set_sign(result, 1);
    }
  } else {
    if (sign_1 && sign_2) {
      if (cmp == 1) {
        binary_sub(big_val1, big_val2, &big_result);
        set_sign(result, 1);
      } else {
        binary_sub(big_val1, big_val2, &big_result);
        set_sign(result, 0);
      }
    } else {
      if (cmp == 1) {
        binary_sub(big_val1, big_val2, &big_result);
        set_sign(result, 0);
      } else {
        binary_sub(big_val1, big_val2, &big_result);
        cmp == 2 ? set_sign(result, 1) : set_sign(result, 0);
      }
    }
  }
  big_result.scale = big_val1.scale;
  flag = big_decimal_to_decimal(big_result, result);

  if (flag == 1 && get_sign(*result) == 1) {
    *result = (s21_decimal){{0, 0, 0, 0}};
    flag = 2;
  } else if (flag == 1 && get_sign(*result) == 0) {
    *result = (s21_decimal){{0, 0, 0, 0}};
    flag = 1;
  }

  return flag;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag = 0;

  int sign_1 = get_sign(value_1);
  int sign_2 = get_sign(value_2);

  int scale1 = get_scale(value_1);
  int scale2 = get_scale(value_2);

  int scale = scale1 + scale2;
  *result = (s21_decimal){{0, 0, 0, 0}};
  big_decimal big_val1 = decimal_to_big(value_1);
  big_decimal big_val2 = decimal_to_big(value_2);
  big_decimal big_result = {{0, 0, 0, 0, 0, 0}, 0};
  binary_mul(big_val1, big_val2, &big_result);
  big_result.scale = scale;
  flag = big_decimal_to_decimal(big_result, result);

  if (sign_1 != sign_2)
    set_sign(result, 1);
  else
    set_sign(result, 0);

  if (flag == 1 && get_sign(*result) == 1) {
    *result = (s21_decimal){{0, 0, 0, 0}};
    flag = 2;
  }

  else if (flag == 1 && get_sign(*result) == 0) {
    *result = (s21_decimal){{0, 0, 0, 0}};
    flag = 1;
  }
  return flag;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  *result = (s21_decimal){{0, 0, 0, 0}};
  int flag = 0;

  int sign_1 = get_sign(value_1);
  int sign_2 = get_sign(value_2);

  int scale1 = get_scale(value_1);
  int scale2 = get_scale(value_2);

  big_decimal big_val1 = decimal_to_big(value_1);
  big_decimal big_val2 = decimal_to_big(value_2);

  big_decimal big_result = {{0, 0, 0, 0, 0, 0}, 0};

  if (compare(big_val2, (big_decimal){{0, 0, 0, 0, 0, 0}, 0}) == 0)
    flag = 3;
  else {
    int scale = scale1 - scale2;

    binary_div(big_val1, big_val2, &big_result);
    int res_scale = big_result.scale + scale;

    big_result.scale = res_scale;
    flag = big_decimal_to_decimal(big_result, result);
    if (flag == 1 && get_sign(*result) == 1) {
      *result = (s21_decimal){{0, 0, 0, 0}};
      flag = 2;
    } else if (flag == 1 && get_sign(*result) == 0) {
      *result = (s21_decimal){{0, 0, 0, 0}};
      flag = 1;
    } else {
      if (sign_1 != sign_2)
        set_sign(result, 1);
      else
        set_sign(result, 0);
    }
  }

  return flag;
}

big_decimal decimal_to_big(s21_decimal from) {
  big_decimal to = {{0, 0, 0, 0, 0, 0}, 0};
  for (int i = 0; i < 3; i++) {
    to.bits[i] = from.bits[i];
  }
  return to;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int flag = 0;
  if (!result) {
    flag = 1;
  } else {
    *result = (s21_decimal){{0, 0, 0, 0}};
    int scale = get_scale(value);
    int sign = get_sign(value);
    big_decimal big_val = decimal_to_big(value);
    big_decimal val2 = {{10, 0, 0, 0, 0, 0}, 0};
    big_decimal big_result = {{0, 0, 0, 0, 0, 0}, 0};

    if (scale > 0) {
      for (int i = 0; i < scale; i++) {
        big_decimal temp_result = {{0, 0, 0, 0, 0, 0}, 0};
        big_decimal tmp_val2 = val2;
        big_result = temp_result;

        int bit_count_val1 = get_bits_count(big_val);
        int bit_count_val2 = get_bits_count(tmp_val2);
        int shift = bit_count_val1 - bit_count_val2;

        helper_for_binary_div(shift, &big_val, val2, &big_result, tmp_val2,
                              temp_result);
        big_val = big_result;
      }

      big_decimal_to_decimal(big_result, result);
    } else {
      *result = value;
    }

    set_sign(result, sign);
  }

  return flag;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  int flag = 0;
  if (!result)
    flag = 1;
  else {
    *result = s21_decimal_get_zero();
    int scale = get_scale(value);
    int sign = get_sign(value);
    big_decimal big_val = decimal_to_big(value);
    big_decimal val2 = {{10, 0, 0, 0, 0, 0}, 0};
    big_decimal big_result = {{0, 0, 0, 0, 0, 0}, 0};
    if (scale > 0) {
      while (scale != 0) {
        scale--;
        big_decimal temp_result = {{0, 0, 0, 0, 0, 0}, 0};
        big_decimal tmp_val2 = val2;
        big_result = temp_result;
        int bit_count_val1 = get_bits_count(big_val);
        int bit_count_val2 = get_bits_count(tmp_val2);
        int shift = bit_count_val1 - bit_count_val2;
        helper_for_binary_div(shift, &big_val, val2, &big_result, tmp_val2,
                              temp_result);
        int remainder = big_val.bits[0];
        if (remainder > 5)
          binary_add(big_result, (big_decimal){{1, 0, 0, 0, 0, 0}, 0},
                     &big_result);
        big_val = big_result;
      }
      big_decimal_to_decimal(big_result, result);
    } else
      *result = value;
    set_sign(result, sign);
  }
  return flag;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  int flag = 0;
  if (!result)
    flag = 1;
  else {
    *result = (s21_decimal){{0, 0, 0, 0}};
    s21_decimal tmp = {{0}};
    for (int i = 0; i < 3; i++) {
      tmp.bits[i] = value.bits[i];
    }
    int sclae = get_scale(value);
    int sign = get_sign(value);
    if (!sign) {
      set_sign(&tmp, 1);
    }
    set_scale(&tmp, sclae);
    *result = tmp;
  }
  return flag;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
  int flag = 0;
  if (!result)
    flag = 1;
  else {
    *result = s21_decimal_get_zero();
    int sign = get_sign(value);
    int scale = get_scale(value);
    s21_decimal temp_result = {{0, 0, 0, 0}};
    s21_truncate(value, &temp_result);
    temp_result.bits[3] = 0;
    if (sign == 1 && scale > 0) {
      s21_add(temp_result, (s21_decimal){{1, 0, 0, 0}}, &temp_result);
    }
    *result = temp_result;
    set_sign(result, sign);
  }
  return flag;
}

s21_decimal s21_decimal_get_zero(void) {
  s21_decimal zero = {{0, 0, 0, 0}};
  return zero;
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int flag = 1;
  if (dst == NULL) {
    flag = 1;
  } else {
    *dst = s21_decimal_get_zero();
    if (src <= 2147483647 && src >= -2147483647) {
      if (src < 0) {
        set_sign(dst, 1);
        dst->bits[0] = (unsigned int)(-src);
      } else {
        dst->bits[0] = (unsigned int)src;
      }
      flag = 0;
    }
  }

  return flag;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int flag = 0;
  if (!dst) {
    flag = 1;
  } else {
    *dst = 0;
    int sign = get_sign(src);
    int scale = get_scale(src);
    if (scale > 28) {
      flag = 1;
    } else {
      unsigned long long int_value = ((unsigned long long)src.bits[2] << 32) |
                                     ((unsigned long long)src.bits[1] << 32) |
                                     src.bits[0];

      for (int i = 0; i < scale; i++) {
        int_value /= 10;
      }
      if (int_value > 2147483647) {
        flag = 1;
      } else {
        if (sign) {
          *dst = (int)int_value * (-1);
        } else {
          *dst = (int)int_value;
        }
      }
    }
  }
  return flag;
}

int is_zero(s21_decimal decimal) {
  int res = 0;
  for (int i = 0; i < 3; i++) {
    if (decimal.bits[i] != 0) {
      res = 1;
    }
  }
  return res;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int flag = 0;

  if (dst == NULL) {
    flag = 1;
  } else if (is_zero(src) == 0) {
    flag = 0;
    *dst = 0.0f;
  } else {
    *dst = 0.0f;
    int scale = get_scale(src);
    int sign = get_sign(src);
    double temp = 0.0;
    for (int i = 2; i >= 0; i--) {
      temp = temp * S21_UMAX_INT + src.bits[i];
    }
    while (scale > 0) {
      temp /= 10.0f;
      scale--;
    }
    *dst = (float)temp;
    if (sign) {
      *dst = -*dst;
    }

    if (fabs(*dst) < 1e-28) {
      *dst = 0.0f;
      flag = 1;
    }

    if (fabs(*dst) > 7.9228162514264337593543950335e28 ||
        fabs(*dst) == INFINITY) {
      *dst = 0.0f;
      flag = 1;
    }
  }
  return flag;
}

int s21_from_float_to_decimal(float src, s21_decimal *decimal) {
  int flag = 0;

  if (isinf(src) || isnan(src) || decimal == NULL || fabs(src) < 1e-28 ||
      fabs(src) > 7.9228162514264337593543950335e28) {
    flag = 1;
  } else if (src == 0.0) {
    decimal->bits[0] = 0;
    decimal->bits[1] = 0;
    decimal->bits[2] = 0;
    set_sign(decimal, 0);
    flag = 0;
  } else {
    *decimal = s21_decimal_get_zero();
    float_to_int_union u;
    u.f = src;
    uint32_t sign = (u.i >> 31) & 1;
    int32_t scale = ((u.i & 0x7F800000) >> 23) - 127;
    double temp = fabs(src);
    int shift = 0;

    for (; shift < 28 && (int)temp / (int)pow(2, 21) == 0;
         temp *= 10, shift++) {
    }
    temp = round(temp);

    if (shift <= 28 && (scale > -94 && scale < 96)) {
      floatbits mant;
      temp = (float)temp;
      for (; fmod(temp, 10) == 0 && shift > 0; shift--, temp /= 10) {
      }
      mant.fl = temp;
      scale = ((mant.ui & 0x7F800000) >> 23) - 127;
      decimal->bits[scale / 32] |= 1 << scale % 32;
      for (int i = scale - 1, j = 22; j >= 0; i--, j--)
        if ((mant.ui & (1 << j)) != 0) decimal->bits[i / 32] |= 1 << i % 32;
      decimal->bits[3] = (sign << 31) | (shift << 16);
    }
  }

  return flag;
}

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int rez = 0;
  if (value_1.bits[0] == 0 && value_1.bits[1] == 0 && value_1.bits[2] == 0 &&
      value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0) {
    rez = 1;
  } else if (value_1.bits[0] == value_2.bits[0] &&
             value_1.bits[1] == value_2.bits[1] &&
             value_1.bits[2] == value_2.bits[2] &&
             value_1.bits[3] == value_2.bits[3])
    rez = 1;
  return rez;
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  int rez = !s21_is_equal(value_1, value_2);
  return rez;
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  big_decimal big_val1 = {{0, 0, 0, 0, 0, 0}, 0},
              big_val2 = {{0, 0, 0, 0, 0, 0}, 0};
  normalize_to_common_scale(value_1, value_2, &big_val1, &big_val2);
  int rez = 0;
  int s1 = get_sign(value_1);
  int s2 = get_sign(value_2);
  if (s21_is_equal(value_1, s21_decimal_get_zero()) &&
      s21_is_equal(value_2, s21_decimal_get_zero())) {
    rez = 0;
  } else if (s1 == 1 && s2 == 0) {
    rez = 1;
  } else if (s1 == 0 && s2 == 1) {
    rez = 0;
  } else if (s1 == 1 && s2 == 1) {
    if (compare(big_val2, big_val1) == 2) rez = 1;
  } else {
    if (compare(big_val1, big_val2) == 2) rez = 1;
  }
  return rez;
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int rez = (s21_is_less(value_1, value_2) || s21_is_equal(value_1, value_2));
  return rez;
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  int rez = s21_is_less(value_2, value_1);
  return rez;
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int rez = (s21_is_less(value_2, value_1) || s21_is_equal(value_1, value_2));
  return rez;
}
