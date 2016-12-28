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
