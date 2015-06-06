#include "kiPasswordFactory.h"

static kiPassword* kiki_pwd_mng_kiPasswordFactory_new_kiPassword(kiPasswordFactory* self);

kiPasswordFactory* kiki_pwd_mng_kiPasswordFactory_init(kiPasswordFactory* _self,
                                                       IV_init_callback iv_initializer,
                                                       kiPasswordRepository* repository,
                                                       kiEncryptor* encryptor,
                                                       kiDecryptor* decryptor) {
	kiPasswordFactory* self = _self ? _self : (kiPasswordFactory*)malloc(sizeof(kiPasswordFactory));

	self->iv_initializer = iv_initializer;
	self->repository = repository;
	self->encryptor = encryptor;
	self->decryptor = decryptor;

	self->new_kiPassword = &kiki_pwd_mng_kiPasswordFactory_new_kiPassword;
}

void kiki_pwd_mng_kiPasswordFactory_free(kiPasswordFactory* self) {
	free(self);
}

kiPassword* kiki_pwd_mng_kiPasswordFactory_new_kiPassword(kiPasswordFactory* self) {
	return kiki_pwd_mng_kiPassword_init(NULL, self->iv_initializer, self->encryptor, self->decryptor, self->repository);
}