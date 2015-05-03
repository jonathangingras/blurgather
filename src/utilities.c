#include <kiki/utilities.h>

size_t kiki_reverse_memlen(unsigned char* str, int end_index) { //like in c++ end index is out of bound
	unsigned char* iterator = str + end_index;
	while(iterator != str && !*--iterator);
	return iterator - str + 1 - (iterator == str && !*iterator ? 1 : 0);
}

void utf_itoa(char* output_array, long value) {
	output_array[0] = 0;
	sprintf(output_array, "%d\0", value);
}

int find_endline(unsigned char* bytes, int n_bytes) {
	int i;
	for(i = 0; i < n_bytes; ++i) {
		if(bytes[i] == '\n') {
			return i;
		}
	}
	return -1;
}

void binstr(char* output_array, unsigned char byte) {
	int i;

	for(i = 0; i < 8; ++i) {
		output_array[i] = (byte & (1 << 7 - i)) ? '1' : '0';
	}

	output_array[8] = 0;
}

int utf8_char_length(unsigned char first_byte) {
	if((first_byte >> 1) << 1 == 0b11111100) {
		return 6;
	} else if((first_byte >> 2) << 2 == 0b11111000) {
		return 5;
	} else if((first_byte >> 3) << 3 == 0b11110000) {
		return 4;
	} else if((first_byte >> 4) << 4 == 0b11100000) {
		return 3;
	} else if((first_byte >> 5) << 5 == 0b11000000) {
		return 2;
	} else if((first_byte >> 7) << 7 == 0) {
		return 1;
	} else {
		return -1;
	}
}