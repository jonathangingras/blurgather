#include "blurgather/utilities.h"

size_t bg_reverse_memlen(const void* str, int end_index) { //like in c++ end index is out of bound
	unsigned char* iterator = ((unsigned char*)str) + end_index;
	while(iterator != str && !*--iterator);
	return iterator - ((unsigned char*)str) + 1 - (iterator == ((unsigned char*)str) && !*iterator ? 1 : 0);
}
