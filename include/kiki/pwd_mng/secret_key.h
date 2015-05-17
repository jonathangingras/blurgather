#ifndef KIKI_PWD_MNG_SECRET_KEY_H
#define KIKI_PWD_MNG_SECRET_KEY_H

#include <stdlib.h>
#include <kiki/kiClass.h>

#ifdef __cplusplus
extern "C" {
#endif

declare_kiclass(kiki_pwd_mng_secret_key_t);

struct kiki_pwd_mng_secret_key_t {
	size_t length;
	unsigned char* value;

	void (* destroy)(kiki_pwd_mng_secret_key_t* self);
	int (* update)(kiki_pwd_mng_secret_key_t* self, const void* value, size_t length);
};

kiki_pwd_mng_secret_key_t* kiki_pwd_mng_secret_key_t_init(kiki_pwd_mng_secret_key_t* self);
void kiki_pwd_mng_secret_key_t_free(kiki_pwd_mng_secret_key_t* self);

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_SECRET_KEY_H
