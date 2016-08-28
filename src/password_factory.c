#include "blurgather/password_factory.h"


bg_password_factory* bg_password_factory_init(bg_password_factory* _self,
                                                       IV_init_callback iv_initializer,
                                                       bg_password_repository* repository,
                                                       bg_encryptor* encryptor,
                                                       bg_decryptor* decryptor) {
	bg_password_factory* self = _self ? _self : (bg_password_factory*)malloc(sizeof(bg_password_factory));

	self->iv_initializer = iv_initializer;
	self->repository = repository;
	self->encryptor = encryptor;
	self->decryptor = decryptor;

  return self;
}

void bg_password_factory_free(bg_password_factory* self) {
	free(self);
}

bg_password* bg_password_factory_new_password(bg_password_factory* self) {
	return bg_password_init(NULL, self->iv_initializer, self->encryptor, self->decryptor, self->repository);
}
