#ifndef KIKI_PWD_MNG_PASSWORD_H
#define KIKI_PWD_MNG_PASSWORD_H

#include <string.h>
#include <stdlib.h>
#include <uuid/uuid.h>

#include <kiki/kiClass.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char* IV_t;

declare_kiclass(kiki_pwd_mng_secret_key_t);
struct kiki_pwd_mng_secret_key_t {
	size_t length;
	unsigned char* value;

	void (* destroy)(kiki_pwd_mng_secret_key_t* self);
	int (* update)(kiki_pwd_mng_secret_key_t* self, const void* value, size_t length);
};

kiki_pwd_mng_secret_key_t* kiki_pwd_mng_secret_key_t_init(kiki_pwd_mng_secret_key_t* self);
void kiki_pwd_mng_secret_key_t_free(kiki_pwd_mng_secret_key_t* self);

declare_kiclass(kiPassword);

declare_kiclass(kiPasswordEncryptor);

struct kiPasswordEncryptor {
	void* object;

	int (* crypt)(kiPasswordEncryptor* self, void* memory, size_t input_length);
	int (* set_iv)(kiPasswordEncryptor* self, IV_t iv);
	int (* set_secret_key)(kiPasswordEncryptor* self, kiki_pwd_mng_secret_key_t secret_key);
};

declare_kiclass(kiPasswordDecryptor);

struct kiPasswordDecryptor {
	void* object;

	int (* decrypt)(kiPasswordDecryptor* self, void* memory, size_t input_length);
	int (* set_iv)(kiPasswordDecryptor* self, IV_t iv);
	int (* set_secret_key)(kiPasswordDecryptor* self, kiki_pwd_mng_secret_key_t secret_key);
};

declare_kiclass(kiPasswordRepository);

struct kiPasswordRepository {
	void* object;

	int (* add)(kiPasswordRepository* self, kiPassword* password);
	int (* remove)(kiPasswordRepository* self, uuid_t password_uuid);

	void (* sort)(kiPasswordRepository* self);

	int (* load)(kiPasswordRepository* self);
	int (* persist)(kiPasswordRepository* self);
};

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

	kiPasswordEncryptor* encryptor;
	kiPasswordDecryptor* decryptor;
	kiPasswordRepository* repository;

	void (* destroy)(kiPassword* self);
	int (* crypt)(kiPassword* self);
	int (* decrypt)(kiPassword* self);
	int (* save)(kiPassword* self);
	int (* update)(kiPassword* self, const char* password_value, size_t password_length);
};

kiPassword* kiki_pwd_mng_kiPassword_init(kiPassword* _self, kiPasswordEncryptor* encryptor,
                                         kiPasswordDecryptor* decryptor, kiPasswordRepository* repository);

void kiki_pwd_mng_kiPassword_free(kiPassword* self);

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_PASSWORD_H
