#ifndef BLURGATHER_PASSWORD_MSGPACK_PERSISTER_H
#define BLURGATHER_PASSWORD_MSGPACK_PERSISTER_H

#include <msgpack.h>

#include <stdlib.h>

#include "password.h"
#include "password_factory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bg_password** bg_password_array;

struct bg_password_msgpack_persister;
typedef struct bg_password_msgpack_persister bg_password_msgpack_persister;

struct bg_password_msgpack_persister {
	void (* destroy)(bg_password_msgpack_persister* self);

	bg_password_repository repository;
	bg_password_factory* password_factory;

	char* persistanceFileName;

	size_t number_passwords;
	bg_password_array password_array;
	size_t allocated_length;
};

bg_password_msgpack_persister* bg_password_msgpack_persister_init(bg_password_msgpack_persister* msgpackPersister,
                                                                         const char* fileName,
                                                                         bg_password_factory* password_factory);

void bg_password_msgpack_persister_free(bg_password_msgpack_persister* msgpackPersister);

#ifdef __cplusplus
}
#endif

#endif //BLURGATHER_PASSWORD_MSGPACK_PERSISTER_H