#ifndef BLURGATHER_PASSWORD_MSGPACK_PERSISTER_H
#define BLURGATHER_PASSWORD_MSGPACK_PERSISTER_H

#include <stdlib.h>
#include "context.h"
#include "password.h"
#include "repository.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bg_password** bg_password_array;

struct bg_password_msgpack_persister;
typedef struct bg_password_msgpack_persister bg_password_msgpack_persister;

struct bg_password_msgpack_persister {
	bg_repository_t repository;
  bg_context *ctx;

  bg_string *persistence_filename;

	size_t number_passwords;
	bg_password_array password_array;
	size_t allocated_length;
};

bg_password_msgpack_persister* bg_password_msgpack_persister_init(bg_password_msgpack_persister* msgpack_persister,
                                                                  bg_context *ctx,
                                                                  const bg_string *filename);

void bg_password_msgpack_persister_free(bg_password_msgpack_persister* msgpack_persister);

#ifdef __cplusplus
}
#endif

#endif /* BLURGATHER_PASSWORD_MSGPACK_PERSISTER_H */
