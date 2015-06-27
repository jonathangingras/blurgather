#ifndef BLURGATHER_ENCRYPTOR_H
#define BLURGATHER_ENCRYPTOR_H

#include "iv.h"
#include "secret_key.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_encryptor;
typedef struct bg_encryptor bg_encryptor;

struct bg_encryptor {
	void* object;

	int (* crypt)(bg_encryptor* self, void* memory, size_t* input_length);
	void (* set_iv)(bg_encryptor* self, IV_t* iv);
	int (* set_secret_key)(bg_encryptor* self, bg_secret_key* secret_key);
};

#ifdef __cplusplus
}
#endif

#endif //BLURGATHER_ENCRYPTOR_H