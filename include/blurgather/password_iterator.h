#ifndef BLURGATHER_PASSWORD_ITERATOR_H
#define BLURGATHER_PASSWORD_ITERATOR_H

#include "password.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_password_iterator;
typedef struct bg_password_iterator bg_password_iterator;

struct bg_password_iterator {
	void* container;

	bg_password** value;

	bg_password** (* previous)(bg_password_iterator* self);
	bg_password** (* next)(bg_password_iterator* self);
};

#ifdef __cplusplus
}
#endif

#endif //BLURGATHER_PASSWORD_ITERATOR_H
