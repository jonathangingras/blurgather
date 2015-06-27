#ifndef BLURGATHER_MCRYPT_CRYPTOR_H
#define BLURGATHER_MCRYPT_CRYPTOR_H

#include <mcrypt.h>

#include "secret_key.h"
#include "encryptor.h"
#include "decryptor.h"
#include "iv.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_mcrypt_cryptor;
typedef struct bg_mcrypt_cryptor bg_mcrypt_cryptor;

struct bg_mcrypt_cryptor {
	void (* destroy)(bg_mcrypt_cryptor* self);

	bg_encryptor encryptor;
	bg_decryptor decryptor;

	bg_secret_key* secret_key;
	IV_t* iv;
};

bg_mcrypt_cryptor* bg_mcrypt_cryptor_init(bg_mcrypt_cryptor* self);
void bg_mcrypt_cryptor_free(bg_mcrypt_cryptor* self);

#ifdef __cplusplus
}
#endif

#endif //BLURGATHER_MCRYPT_CRYPTOR_H
