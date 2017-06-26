#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <blurgather/string.h>

struct bg_string {
  size_t length;
};

#define STR_DATA(str) ((const char *)((str)+1))
#define STR_NUL(str) ( ((char *)((str)+1)) + ((str)->length) )
#define STR_APPEND_NUL(str) *STR_NUL(str) = 0

bg_string *bg_string_new() {
  bg_string *str = malloc(sizeof(bg_string) + 1);
  str->length = 0;
  STR_APPEND_NUL(str);
  return str;
}

bg_string *bg_string_filled_with_length(char character, size_t length) {
  bg_string *str = malloc(sizeof(bg_string) + length + 1);
  str->length = length;
  STR_APPEND_NUL(str);
  memset((void*)STR_DATA(str), character, length);
  return str;
}

bg_string *bg_string_from_char_array(const char *array, size_t length) {
  bg_string *str = malloc(sizeof(bg_string) + length + 1);

  memcpy((void*)STR_DATA(str), array, length);
  str->length = length;
  STR_APPEND_NUL(str);

  return str;
}

bg_string *bg_string_from_str(const char *array) {
  return bg_string_from_char_array(array, strlen(array));
}

bg_string *bg_string_copy(const bg_string *copied) {
  return bg_string_from_char_array(bg_string_data(copied), bg_string_length(copied));
}

void bg_string_clean(bg_string *str) {
  memset((void*)STR_DATA(str), 0, str->length);
}

bg_string *bg_string_strip_nuls(bg_string **str) {
  if((*str)->length == 0) { return *str; }

  size_t to_remove = 0;
  char *last = ((char*)(STR_NUL(*str))) - 1;
  while(1) {
    if(*last == 0) { ++to_remove; }
    else { break; }
    --last;
    if(to_remove == (*str)->length) { break; }
  }

  if(to_remove == 0) { return *str; }

  bg_string *stripped = bg_string_filled_with_length(0, (*str)->length - to_remove);
  memcpy((void*)STR_DATA(stripped), STR_DATA(*str), stripped->length);

  bg_string_clean_free(*str);
  *str = stripped;
  return stripped;
}

size_t bg_string_length(const bg_string *str) {
  return str->length;
}

int bg_string_empty(const bg_string *str) {
  return str->length == 0;
}

const char *bg_string_data(const bg_string *str) {
  return STR_DATA(str);
}

int bg_string_compare(const bg_string *str1, const bg_string *str2) {
  if(str1->length == str2->length) {
    return memcmp(STR_DATA(str1), STR_DATA(str2), str1->length);
  }
  return -1;
}

bg_string *bg_string_cat(bg_string **str, const bg_string *catted) {
  return bg_string_cat_char_array(str, STR_DATA(catted), catted->length);
}

bg_string *bg_string_cat_char_array(bg_string **str, const char *catted, size_t length) {
  *str = realloc(*str, sizeof(bg_string) + (*str)->length + length + 1);
  memcpy((void*)STR_DATA(*str) + (*str)->length, catted, length);
  (*str)->length += length;
  STR_APPEND_NUL(*str);
  return *str;
}

bg_string *bg_string_plus(bg_string *str1, bg_string *str2) {
  bg_string *res = str1;
  bg_string_cat(&res, str2);
  bg_string_clean_free(str2);
  return res;
}

bg_string *bg_string_from_decimal(long decimal) {
  unsigned int max_strlen = ((unsigned int)ceil(log10(LONG_MAX))) + 1 + 1; /* possible minus sign + NUL */
  char array[max_strlen];
  memset(array, 0, max_strlen);
  sprintf(array, "%ld", decimal);
  return bg_string_from_str(array);
}

int bg_string_split_after(const bg_string *str, size_t index, bg_string **left, bg_string **right) {
  if(index > str->length) {
    return -1;
  }

  *left = bg_string_from_char_array(STR_DATA(str), index);
  *right = bg_string_from_char_array(STR_DATA(str) + index, str->length - index);

  return 0;
}
