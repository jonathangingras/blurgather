#include "blurgather/string.h"
#include <string.h>
#include <sweetgreen/sweetgreen.h>

#define VALID_STRING "hello world!"
#define VALID_STRING2 "hello world "

sweetgreen_test_define(string, can_instantiate_empty_string) {
  bg_string *str = bg_string_new();

  sweetgreen_expect_not_same_address(NULL, str);
  sweetgreen_expect_zero(bg_string_length(str));
}

sweetgreen_test_define(string, can_instantiate_filled_string_0_char) {
  bg_string *str = bg_string_filled_with_length('1', 0);

  sweetgreen_expect_not_same_address(NULL, str);
  sweetgreen_expect_equal(0, bg_string_length(str));
  sweetgreen_expect_equal_memory("", bg_string_data(str), 1);
}

sweetgreen_test_define(string, can_instantiate_filled_string_one_char) {
  bg_string *str = bg_string_filled_with_length('1', 1);

  sweetgreen_expect_not_same_address(NULL, str);
  sweetgreen_expect_equal(1, bg_string_length(str));
  sweetgreen_expect_equal_memory("1", bg_string_data(str), 2);
}

sweetgreen_test_define(string, can_instantiate_filled_string_multiple_chars) {
  bg_string *str = bg_string_filled_with_length('1', 10);

  sweetgreen_expect_not_same_address(NULL, str);
  sweetgreen_expect_equal(10, bg_string_length(str));
  sweetgreen_expect_equal_memory("1111111111", bg_string_data(str), 11);
}

sweetgreen_test_define(string, can_instantiate_string_from_char_array) {
  char array[] = VALID_STRING;

  bg_string *str = bg_string_from_char_array(array, strlen(array));

  sweetgreen_expect_equal_memory(array, bg_string_data(str), strlen(array) + 1);
}

sweetgreen_test_define(string, length_is_appropriate) {
  char array[] = VALID_STRING;

  bg_string *str = bg_string_from_char_array(array, strlen(array));

  sweetgreen_expect_equal(bg_string_length(str), strlen(array));
}

sweetgreen_test_define(string, comparison_returns_0_when_equal_strings) {
  char array[] = VALID_STRING;
  bg_string *str1 = bg_string_from_char_array(array, strlen(array));
  bg_string *str2 = bg_string_from_char_array(array, strlen(array));

  sweetgreen_expect_zero(bg_string_compare(str1, str2));
}

sweetgreen_test_define(string, comparison_returns_non_0_when_non_equal_strings__case_same_length) {
  char array[] = VALID_STRING;
  char array2[] = VALID_STRING2;
  bg_string *str1 = bg_string_from_char_array(array, strlen(array));
  bg_string *str2 = bg_string_from_char_array(array2, strlen(array2));

  sweetgreen_expect_non_zero(bg_string_compare(str1, str2));
}

sweetgreen_test_define(string, comparison_returns_non_0_when_non_equal_strings__case_not_same_length) {
  char array[] = VALID_STRING;
  char array2[] = VALID_STRING2;
  bg_string *str1 = bg_string_from_char_array(array, strlen(array));
  bg_string *str2 = bg_string_from_char_array(array2, strlen(array2) - 1);

  sweetgreen_expect_non_zero(bg_string_compare(str1, str2));
}

sweetgreen_test_define(string, can_concatenate_a_string) {
  char array[] = VALID_STRING;
  char array2[] = VALID_STRING2;
  char comparer[strlen(VALID_STRING) + strlen(VALID_STRING2) + 1];
  comparer[0] = 0;
  strcat(comparer, array);
  strcat(comparer, array2);

  bg_string *str = bg_string_from_char_array(array, strlen(array));
  bg_string *catted = bg_string_from_char_array(array2, strlen(array2));

  sweetgreen_expect_not_null(bg_string_cat(&str, catted));
  sweetgreen_expect_equal_memory(comparer, bg_string_data(str), strlen(comparer) + 1);
  sweetgreen_expect_equal(strlen(comparer), bg_string_length(str));
}

sweetgreen_test_define(string, can_concatenate_a_char_array) {
  char array[] = VALID_STRING;
  char catted[] = VALID_STRING2;
  char comparer[strlen(VALID_STRING) + strlen(VALID_STRING2) + 1];
  comparer[0] = 0;
  strcat(comparer, array);
  strcat(comparer, catted);

  bg_string *str = bg_string_from_char_array(array, strlen(array));

  sweetgreen_expect_not_null(bg_string_cat_char_array(&str, catted, strlen(catted)));
  sweetgreen_expect_equal_memory(comparer, bg_string_data(str), strlen(comparer) + 1);
  sweetgreen_expect_equal(strlen(comparer), bg_string_length(str));
}

sweetgreen_test_define(string, can_strip_nul_characters_at_end_when_empty_string) {
  bg_string *str = bg_string_new();

  bg_string_strip_nuls(&str);

  sweetgreen_expect_equal(0, bg_string_length(str));
  sweetgreen_expect_equal_memory("", bg_string_data(str), 1);
}

sweetgreen_test_define(string, can_strip_nul_characters_at_end_when_0_nul_character) {
  char array[] = {'1', '1', '1'};
  bg_string *str = bg_string_from_char_array(array, 3);

  bg_string_strip_nuls(&str);

  sweetgreen_expect_equal(3, bg_string_length(str));
  sweetgreen_expect_equal_memory("111", bg_string_data(str), 4);
}

sweetgreen_test_define(string, can_strip_nul_characters_at_end_when_1_nul_character_and_rest_is_empty) {
  char array[] = {0};
  bg_string *str = bg_string_from_char_array(array, 1);

  sweetgreen_expect_equal(1, bg_string_length(str));

  bg_string_strip_nuls(&str);

  sweetgreen_expect_equal(0, bg_string_length(str));
  sweetgreen_expect_equal_memory("", bg_string_data(str), 1);
}

sweetgreen_test_define(string, can_strip_nul_characters_at_end_when_1_nul_character) {
  char array[] = {'1', '1', '1', 0};
  bg_string *str = bg_string_from_char_array(array, 4);

  sweetgreen_expect_equal(4, bg_string_length(str));

  bg_string_strip_nuls(&str);

  sweetgreen_expect_equal(3, bg_string_length(str));
  sweetgreen_expect_equal_memory("111", bg_string_data(str), 4);
}

sweetgreen_test_define(string, can_strip_nul_characters_at_end_when_2_nul_characters) {
  char array[] = {'1', '1', '1', 0, 0};
  bg_string *str = bg_string_from_char_array(array, 5);

  sweetgreen_expect_equal(5, bg_string_length(str));

  bg_string_strip_nuls(&str);

  sweetgreen_expect_equal(3, bg_string_length(str));
  sweetgreen_expect_equal_memory("111", bg_string_data(str), 4);
}

sweetgreen_test_define(string, can_strip_nul_characters_at_end_when_multiple_nul_characters) {
  char array[] = {'1', '1', '1', 0, 0, 0, 0, 0, 0};
  bg_string *str = bg_string_from_char_array(array, 9);

  sweetgreen_expect_equal(9, bg_string_length(str));

  bg_string_strip_nuls(&str);

  sweetgreen_expect_equal(3, bg_string_length(str));
  sweetgreen_expect_equal_memory("111", bg_string_data(str), 4);
}

sweetgreen_test_define(string, can_split_in_middle) {
  char *array = "hello, i am some string";
  bg_string *str = bg_string_from_str(array);

  bg_string *lhs, *rhs;
  bg_string_split_after(str, 8, &lhs, &rhs);

  sweetgreen_expect_equal_string("hello, i", bg_string_data(lhs));
  sweetgreen_expect_equal(8, bg_string_length(lhs));
  sweetgreen_expect_equal_string(" am some string", bg_string_data(rhs));
  sweetgreen_expect_equal(15, bg_string_length(rhs));
}

sweetgreen_test_define(string, can_split_at_beginning) {
  char *array = "hello, i am some string";
  bg_string *str = bg_string_from_str(array);

  bg_string *lhs, *rhs;
  bg_string_split_after(str, 0, &lhs, &rhs);

  sweetgreen_expect_equal_string("", bg_string_data(lhs));
  sweetgreen_expect_equal(0, bg_string_length(lhs));
  sweetgreen_expect_equal_string(array, bg_string_data(rhs));
  sweetgreen_expect_equal(strlen(array), bg_string_length(rhs));
}

sweetgreen_test_define(string, can_split_at_end) {
  char *array = "hello, i am some string";
  bg_string *str = bg_string_from_str(array);

  bg_string *lhs, *rhs;
  bg_string_split_after(str, strlen(array), &lhs, &rhs);

  sweetgreen_expect_equal_string(array, bg_string_data(lhs));
  sweetgreen_expect_equal(strlen(array), bg_string_length(lhs));
  sweetgreen_expect_equal_string("", bg_string_data(rhs));
  sweetgreen_expect_equal(0, bg_string_length(rhs));
}
