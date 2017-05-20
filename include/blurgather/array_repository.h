#ifndef BLURGATHER_PASSWORD_ARRAY_REPOSITORY_H
#define BLURGATHER_PASSWORD_ARRAY_REPOSITORY_H

#include <stdlib.h>
#include "context.h"
#include "password.h"
#include "repository.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bg_password** bg_password_array;

struct bg_password_array_repository;
typedef struct bg_password_array_repository bg_password_array_repository;

struct bg_password_array_repository {
  bg_context *ctx;

	bg_repository_t repository;

	size_t number_passwords;
	bg_password_array password_array;
	size_t allocated_length;
};

bg_password_array_repository* bg_password_array_repository_init(bg_password_array_repository* msgpack_persister, bg_context *ctx);

void bg_password_array_repository_free(bg_password_array_repository* msgpack_persister);

bg_repository_t *bg_password_array_repository_repository(bg_password_array_repository *msgpack_persister);

#ifdef __cplusplus
}
#endif

#endif /* BLURGATHER_PASSWORD_ARRAY_REPOSITORY_H */
