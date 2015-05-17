#ifndef KIKI_PWD_MNG_KIPASSWORDMCRYPTENCRYPTOR_H
#define KIKI_PWD_MNG_KIPASSWORDMCRYPTENCRYPTOR_H

#include <mcrypt.h>
#include <kiki/kiClass.h>
#include "secret_key.h"
#include "kiEncryptor.h"
#include "kiDecryptor.h"

#ifdef __cplusplus
extern "C" {
#endif

declare_kiclass(kiMCryptEncryptor);

struct kiMCryptEncryptor {
	kiEncryptor encryptor;
	kiDecryptor decryptor;

	void (* destroy)(kiMCryptEncryptor* self);

	kiki_pwd_mng_secret_key_t* secret_key;
	unsigned char iv[16];
};

kiMCryptEncryptor* kiki_pwd_mng_kiMCryptEncryptor_init(kiMCryptEncryptor* self);
void kiki_pwd_mng_kiMCryptEncryptor_free(kiMCryptEncryptor* self);

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_KIPASSWORDMCRYPTENCRYPTOR_H
