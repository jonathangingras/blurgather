#ifndef _BLURGATHER_STRING_H_
#define _BLURGATHER_STRING_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bg_string;
typedef struct bg_string bg_string;

bg_string *bg_string_new();
bg_string *bg_string_filled_with_length(char character, size_t length);
bg_string *bg_string_from_char_array(const char *array, size_t length);
bg_string *bg_string_from_str(const char *array);
bg_string *bg_string_copy(const bg_string*);

#define bg_string_free free
#define bg_string_clean_free(str) bg_string_clean(str); bg_string_free(str)

const char *bg_string_data(const bg_string *str);
size_t bg_string_length(const bg_string *str);
int bg_string_empty(const bg_string *str);

void bg_string_clean(bg_string *str);
bg_string *bg_string_strip_nuls(bg_string **str);
bg_string *bg_string_cat(bg_string **str, const bg_string *catted);
bg_string *bg_string_cat_char_array(bg_string **str, const char *catted, size_t length);

bg_string *bg_string_plus(bg_string *str1, bg_string *str2);

bg_string *bg_string_from_decimal(long decimal);

int bg_string_split_after(const bg_string *str, size_t index, bg_string **left, bg_string **right);

int bg_string_compare(const bg_string *str1, const bg_string *str2);

#define bg_string_replace(old, new) bg_string_free(old); old = new
#define bg_string_clean_replace(old, new) bg_string_clean_free(old); old = new

#ifdef __cplusplus
}
#endif

#endif
