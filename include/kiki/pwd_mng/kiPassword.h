#ifndef KIKI_PWD_MNG_PASSWORD_H
#define KIKI_PWD_MNG_PASSWORD_H

#include <string.h>
#include <stdlib.h>
#include <uuid/uuid.h>

#include <kiki/kiClass.h>

#include "iv.h"
#include "secret_key.h"
#include "kiEncryptor.h"
#include "kiDecryptor.h"

#ifdef __cplusplus
extern "C" {
#endif

declare_kiclass(kiPassword);

#include "kiPasswordRepository.h"

#define KIKI_PWD_MAX_NAME_LEN 100
#define KIKI_PWD_MAX_DESCRIPTION_LEN 500
#define KIKI_PWD_MAX_VALUE_LEN 256

struct kiPassword {
	uuid_t uuid;

	IV_t iv;
	char* name; //max 100 bytes
	char* description; //max 500 bytes
	char* value; //max 256 bytes
	size_t value_length; //actual length

	int crypted;

	kiEncryptor* encryptor;
	kiDecryptor* decryptor;
	kiPasswordRepository* repository;

	void (* destroy)(kiPassword* self);
	int (* crypt)(kiPassword* self);
	int (* decrypt)(kiPassword* self);
	int (* save)(kiPassword* self);
	int (* update)(kiPassword* self, const char* password_value, size_t password_length);
};

kiPassword* kiki_pwd_mng_kiPassword_init(kiPassword* _self, kiEncryptor* encryptor,
                                         kiDecryptor* decryptor, kiPasswordRepository* repository);

void kiki_pwd_mng_kiPassword_free(kiPassword* self);

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_PASSWORD_H
