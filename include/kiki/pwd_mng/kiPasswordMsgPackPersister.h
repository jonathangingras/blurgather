#ifndef KIKI_PWD_MNG_KIPASSWORDJSONPERSISTER_H
#define KIKI_PWD_MNG_KIPASSWORDJSONPERSISTER_H

#include <stdlib.h>
#include <kiki/kiClass.h>
#include <msgpack.h>
#include "kiPassword.h"
#include "kiPasswordFactory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef kiPassword** kiPasswordArray;

declare_kiclass(kiPasswordMsgPackPersister);

struct kiPasswordMsgPackPersister {
	kiPasswordRepository repository;
	kiPasswordFactory* password_factory;

	char* persistanceFileName;

	size_t number_passwords;
	kiPasswordArray password_array;
	size_t allocated_length;

	void (* destroy)(kiPasswordMsgPackPersister* self);
};

kiPasswordMsgPackPersister* kiki_pwd_mng_kiPasswordMsgPackPersister_init(kiPasswordMsgPackPersister* msgpackPersister,
                                                                         const char* fileName,
                                                                         kiPasswordFactory* password_factory);

void kiki_pwd_mng_kiPasswordMsgPackPersister_free(kiPasswordMsgPackPersister* msgpackPersister);

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_KIPASSWORDJSONPERSISTER_H