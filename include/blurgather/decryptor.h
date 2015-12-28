#ifndef BLURGATHER_DECRYPTOR_H
#define BLURGATHER_DECRYPTOR_H

#include "iv.h"
#include "secret_key.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_decryptor;
typedef struct bg_decryptor bg_decryptor;

struct bg_decryptor {
	void* object;

	int (* decrypt)(bg_decryptor* self, void* memory, size_t* input_length);
	void (* set_iv)(bg_decryptor* self, IV_t* iv);
	int (* set_secret_key)(bg_decryptor* self, bg_secret_key* secret_key);
};

#ifdef __cplusplus
}
#endif

#endif //BLURGATHER_DECRYPTOR_H
