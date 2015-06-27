#ifndef BLURGATHER_PASSWORD_H
#define BLURGATHER_PASSWORD_H

#include <string.h>
#include <stdlib.h>
#include <uuid/uuid.h>

#include "iv.h"
#include "secret_key.h"
#include "encryptor.h"
#include "decryptor.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_password;
typedef struct bg_password bg_password;

#include "password_repository.h"

#define BLURGATHER_PWD_MAX_NAME_LEN 100
#define BLURGATHER_PWD_MAX_DESCRIPTION_LEN 500
#define BLURGATHER_PWD_MAX_VALUE_LEN 256

struct bg_password {
	void (* destroy)(bg_password* self);

	uuid_t uuid;

	IV_t iv;
	char* name; //max 100 bytes
	char* description; //max 500 bytes
	char* value; //max 256 bytes
	size_t value_length; //actual length

	int crypted;

	bg_encryptor* encryptor;
	bg_decryptor* decryptor;
	bg_password_repository* repository;

	int (* crypt)(bg_password* self);
	int (* decrypt)(bg_password* self);
	int (* save)(bg_password* self);
	int (* update)(bg_password* self, const char* password_value, size_t password_length);
};

bg_password* bg_password_init(bg_password* _self, IV_init_callback iv_initializer, bg_encryptor* encryptor,
                                         bg_decryptor* decryptor, bg_password_repository* repository);

void bg_password_free(bg_password* self);

#ifdef __cplusplus
}
#endif

#endif //BLURGATHER_PASSWORD_H
