#ifndef KIKI_PWD_MNG_KIPASSWORDMCRYPTENCRYPTOR_H
#define KIKI_PWD_MNG_KIPASSWORDMCRYPTENCRYPTOR_H

#include "kiPassword.h"
#include <mcrypt.h>

#ifdef __cplusplus
extern "C" {
#endif

declare_kiclass(kiPasswordMCryptEncryptor);

struct kiPasswordMCryptEncryptor {
	kiPasswordEncryptor encryptor;
	kiPasswordDecryptor decryptor;

	void (* destroy)(kiPasswordMCryptEncryptor* self);

	kiki_pwd_mng_secret_key_t secret_key;
	unsigned char iv[16];
};

kiPasswordMCryptEncryptor* kiki_pwd_mng_kiPasswordMCryptEncryptor_init(kiPasswordMCryptEncryptor* self);
void kiki_pwd_mng_kiPasswordMCryptEncryptor_free(kiPasswordMCryptEncryptor* self);

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_KIPASSWORDMCRYPTENCRYPTOR_H
