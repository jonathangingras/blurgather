#ifndef BLURGATHER_IV_H
#define BLURGATHER_IV_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct IV_t;
typedef struct IV_t IV_t;

struct IV_t {
	void (* destroy)(IV_t* self);

	unsigned char* value;
	size_t length;

	int (* randomize)(IV_t* self);
};

typedef IV_t* (*IV_init_callback)(IV_t* iv);

#ifdef __cplusplus
}
#endif

#endif //BLURGATHER_IV_H
