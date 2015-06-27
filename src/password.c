#include <kiki/utilities.h>

#include "password.h"

static void bg_password_destroy(bg_password* self);
static int bg_password_crypt(bg_password* self);
static int bg_password_decrypt(bg_password* self);
static int bg_password_save(bg_password* self);
static int bg_password_update(bg_password* self, const char* password_value, size_t password_length);

bg_password* bg_password_init(bg_password* _self, IV_init_callback iv_initializer, bg_encryptor* encryptor,
                                         bg_decryptor* decryptor, bg_password_repository* repository) {
	bg_password* self = _self;
	if(!_self) {
		self = (bg_password*) malloc(sizeof(bg_password));
	}

	self->encryptor = encryptor;
	self->decryptor = decryptor;
	self->repository = repository;

	uuid_generate(self->uuid);
	iv_initializer(&self->iv);
	self->name = (char*) calloc(BLURGATHER_PWD_MAX_NAME_LEN, sizeof(char));
	self->description = (char*) calloc(BLURGATHER_PWD_MAX_DESCRIPTION_LEN, sizeof(char));
	self->value = (char*) calloc(BLURGATHER_PWD_MAX_VALUE_LEN, sizeof(char));
	self->value_length = 0;
	self->crypted = 0;

	self->destroy = &bg_password_destroy;
	self->crypt = &bg_password_crypt;
	self->decrypt = &bg_password_decrypt;
	self->save = &bg_password_save;
	self->update = &bg_password_update;

	return self;
}

void bg_password_free(bg_password* self) {
	self->destroy(self);
	free(self);
}


//methods

void bg_password_destroy(bg_password* self) {
	self->iv.destroy(&self->iv);
	free(self->name);
	free(self->description);
	free(self->value);
}

#define clean_memcpy(value, buffer, length) \
    memset(value, 0, BLURGATHER_PWD_MAX_VALUE_LEN);\
    memcpy(value, buffer, length);

int bg_password_crypt(bg_password* self) {
	if(!self->encryptor) { return -1; }
	if(self->crypted) { return 1; }
	char buffer[BLURGATHER_PWD_MAX_VALUE_LEN];
	clean_memcpy(buffer, self->value, self->value_length);

	self->encryptor->set_iv(self->encryptor, &self->iv);

	int error_value = 0;
	if((error_value = self->encryptor->crypt(self->encryptor, buffer, &self->value_length))) { return error_value; }
	self->crypted = 1;

	clean_memcpy(self->value, buffer, self->value_length);
	return 0;
}

int bg_password_decrypt(bg_password* self) {
	if(!self->decryptor) { return -1; }
	if(!self->crypted) { return 1; }
	char buffer[BLURGATHER_PWD_MAX_VALUE_LEN];
	clean_memcpy(buffer, self->value, self->value_length);

	self->decryptor->set_iv(self->decryptor, &self->iv);

	if(self->decryptor->decrypt(self->decryptor, buffer, &self->value_length)) { return 1; }
	self->crypted = 0;

	clean_memcpy(self->value, buffer, self->value_length);
	return 0;
}

int bg_password_save(bg_password* self) {
	if(!self->repository) { return -1; }
	return self->repository->add(self->repository, self);
}

int bg_password_update(bg_password* self, const char* password_value, size_t password_length) {
	clean_memcpy(self->value, password_value, password_length);
	self->value_length = password_length;
	self->crypted = 0;
	self->iv.randomize(&self->iv);

	return 0;
}