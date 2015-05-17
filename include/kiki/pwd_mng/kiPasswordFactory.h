#ifndef KIKI_PWD_MNG_KIPASSWORDFACTORY_H
#define KIKI_PWD_MNG_KIPASSWORDFACTORY_H

#include "kiPassword.h"
#include "kiEncryptor.h"
#include "kiDecryptor.h"

#ifdef __cplusplus
extern "C" {
#endif

declare_kiclass(kiPasswordFactory);

struct kiPasswordFactory {
	kiPasswordRepository* repository;
	kiEncryptor* encryptor;
	kiDecryptor* decryptor;

	kiPassword* (* new_kiPassword)(kiPasswordFactory* self);
};

kiPasswordFactory* kiki_pwd_mng_kiPasswordFactory_init(kiPasswordFactory* self,
                                                       kiPasswordRepository* repository,
                                                       kiEncryptor* encryptor,
                                                       kiDecryptor* decryptor);

void kiki_pwd_mng_kiPasswordFactory_free(kiPasswordFactory* self);

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_KIPASSWORDFACTORY_H
