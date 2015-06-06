#ifndef KIKI_PWD_MNG_KIPASSWORDDECRYPTOR_H
#define KIKI_PWD_MNG_KIPASSWORDDECRYPTOR_H

#include <kiki/kiClass.h>
#include "iv.h"
#include "secret_key.h"

#ifdef __cplusplus
extern "C" {
#endif

declare_kiclass(kiDecryptor);

kiclass(kiDecryptor) {
	void* object;

	int (* decrypt)(kiDecryptor* self, void* memory, size_t* input_length);
	void (* set_iv)(kiDecryptor* self, IV_t* iv);
	int (* set_secret_key)(kiDecryptor* self, kiki_pwd_mng_secret_key_t* secret_key);
};

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_KIPASSWORDDECRYPTOR_H
