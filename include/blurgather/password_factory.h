#ifndef BLURGATHER_PASSWORD_FACTORY_H
#define BLURGATHER_PASSWORD_FACTORY_H

#include "password.h"
#include "encryptor.h"
#include "decryptor.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_password_factory;
typedef struct bg_password_factory bg_password_factory;

struct bg_password_factory {
	IV_init_callback iv_initializer;
	bg_password_repository* repository;
	bg_encryptor* encryptor;
	bg_decryptor* decryptor;

	bg_password* (* new_password)(bg_password_factory* self);
};

bg_password_factory* bg_password_factory_init(bg_password_factory* self,
                                                       IV_init_callback ivInitCallback,
                                                       bg_password_repository* repository,
                                                       bg_encryptor* encryptor,
                                                       bg_decryptor* decryptor);

void bg_password_factory_free(bg_password_factory* self);

#ifdef __cplusplus
}
#endif

#endif //BLURGATHER_PASSWORD_FACTORY_H
