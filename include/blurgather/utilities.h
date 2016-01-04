#ifndef KIKI_PWD_MNG_UTILITIES_H
#define KIKI_PWD_MNG_UTILITIES_H

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t bg_reverse_memlen(const void* str, int end_index);

#define lened_str(str) \
	str, strlen(str) + 1

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_UTILITIES_H
