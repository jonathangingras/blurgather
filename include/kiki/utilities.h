#ifndef KIKI_PWD_MNG_UTILITIES_H
#define KIKI_PWD_MNG_UTILITIES_H

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t kiki_reverse_memlen(unsigned char* str, int end_index);
void utf_itoa(char* output_array, long value);
int find_endline(unsigned char* bytes, int n_bytes);
void binstr(char* output_array, unsigned char byte);
int utf8_char_length(unsigned char first_byte);

#define lened_str(str) \
	str, strlen(str) + 1

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_UTILITIES_H
