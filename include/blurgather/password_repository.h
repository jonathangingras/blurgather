#ifndef BLURGATHER_PASSWORD_REPOSITORY_H
#define BLURGATHER_PASSWORD_REPOSITORY_H

#include <uuid/uuid.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bg_password_repository;
typedef struct bg_password_repository bg_password_repository;

#include "password.h"
#include "password_iterator.h"

struct bg_password_repository {
	void* object;

	int (* add)(bg_password_repository* self, bg_password* password);
	int (* remove)(bg_password_repository* self, uuid_t password_uuid);

	void (* sort)(bg_password_repository* self);
	bg_password_iterator (* begin)(bg_password_repository* self);
	bg_password_iterator (* end)(bg_password_repository* self);

	int (* load)(bg_password_repository* self);
	int (* persist)(bg_password_repository* self);
};

#ifdef __cplusplus
}
#endif

#endif //BLURGATHER_PASSWORD_REPOSITORY_H
