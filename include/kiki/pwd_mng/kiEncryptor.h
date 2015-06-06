#ifndef KIKI_PWD_MNG_KIPASSWORDENCRYPTOR_H
#define KIKI_PWD_MNG_KIPASSWORDENCRYPTOR_H

#include <kiki/kiClass.h>
#include "iv.h"
#include "secret_key.h"

#ifdef __cplusplus
extern "C" {
#endif

declare_kiclass(kiEncryptor);

kiclass(kiEncryptor) {
	void* object;

	int (* crypt)(kiEncryptor* self, void* memory, size_t* input_length);
	void (* set_iv)(kiEncryptor* self, IV_t* iv);
	int (* set_secret_key)(kiEncryptor* self, kiki_pwd_mng_secret_key_t* secret_key);
};

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_KIPASSWORDENCRYPTOR_H