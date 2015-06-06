#ifndef KIKI_PWD_MNG_IV_H
#define KIKI_PWD_MNG_IV_H

#include <stdlib.h>
#include <kiki/kiClass.h>

#ifdef __cplusplus
extern "C" {
#endif

declare_kiclass(IV_t);

kiclass(IV_t) {
	unsigned char* value;
	size_t length;

	void (* destroy)(IV_t* self);
	int (* randomize)(IV_t* self);
};

typedef IV_t* (*IV_init_callback)(IV_t* iv);

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_IV_H
