#include <kiki/pwd_mng/kiPassword.h>
#include <kiki/utilities.h>

static void kiki_pwd_mng_kiPassword_destroy(kiPassword* self);
static int kiki_pwd_mng_kiPassword_crypt(kiPassword* self);
static int kiki_pwd_mng_kiPassword_decrypt(kiPassword* self);
static int kiki_pwd_mng_kiPassword_save(kiPassword* self);
static int kiki_pwd_mng_kiPassword_update(kiPassword* self, const char* password_value, size_t password_length);

kiPassword* kiki_pwd_mng_kiPassword_init(kiPassword* _self, kiPasswordEncryptor* encryptor,
                                         kiPasswordDecryptor* decryptor, kiPasswordRepository* repository) {
	kiPassword* self = _self;
	if(!_self) {
		self = (kiPassword*) malloc(sizeof(kiPassword));
	}

	self->encryptor = encryptor;
	self->decryptor = decryptor;
	self->repository = repository;

	uuid_generate(self->uuid);
	self->iv = (unsigned char*) calloc(16, sizeof(unsigned char));
	self->name = (char*) calloc(KIKI_PWD_MAX_NAME_LEN, sizeof(char));
	self->description = (char*) calloc(KIKI_PWD_MAX_DESCRIPTION_LEN, sizeof(char));
	self->value = (char*) calloc(KIKI_PWD_MAX_VALUE_LEN, sizeof(char));
	self->value_length = 0;
	self->crypted = 0;

	self->destroy = &kiki_pwd_mng_kiPassword_destroy;
	self->crypt = &kiki_pwd_mng_kiPassword_crypt;
	self->decrypt = &kiki_pwd_mng_kiPassword_decrypt;
	self->save = &kiki_pwd_mng_kiPassword_save;
	self->update = &kiki_pwd_mng_kiPassword_update;

	return self;
}

void kiki_pwd_mng_kiPassword_free(kiPassword* self) {
	self->destroy(self);
	free(self);
}


//methods

void kiki_pwd_mng_kiPassword_destroy(kiPassword* self) {
	free(self->iv);
	free(self->name);
	free(self->description);
	free(self->value);
}

#define clean_memcpy(value, buffer, length) \
    memset(value, 0, KIKI_PWD_MAX_VALUE_LEN);\
    memcpy(value, buffer, length);

int kiki_pwd_mng_kiPassword_crypt(kiPassword* self) {
	if(!self->encryptor) { return -1; }
	if(self->crypted) { return 1; }
	char buffer[KIKI_PWD_MAX_VALUE_LEN];
	clean_memcpy(buffer, self->value, self->value_length);

	if(self->encryptor->crypt(self->encryptor, buffer, self->value_length)) { return 1; }
	self->value_length = kiki_reverse_memlen(buffer, KIKI_PWD_MAX_VALUE_LEN);
	self->crypted = 1;

	clean_memcpy(self->value, buffer, self->value_length);
	return 0;
}

int kiki_pwd_mng_kiPassword_decrypt(kiPassword* self) {
	if(!self->decryptor) { return -1; }
	if(!self->crypted) { return 1; }
	char buffer[KIKI_PWD_MAX_VALUE_LEN];
	clean_memcpy(buffer, self->value, self->value_length);

	if(self->decryptor->decrypt(self->decryptor, buffer, self->value_length)) { return 1; }
	self->value_length = kiki_reverse_memlen(buffer, KIKI_PWD_MAX_VALUE_LEN);
	self->crypted = 0;

	clean_memcpy(self->value, buffer, self->value_length);
	return 0;
}

int kiki_pwd_mng_kiPassword_save(kiPassword* self) {
	if(!self->repository) { return -1; }
	return self->repository->add(self->repository, self);
}

int kiki_pwd_mng_kiPassword_update(kiPassword* self, const char* password_value, size_t password_length) {
	clean_memcpy(self->value, password_value, password_length);
	self->value_length = password_length;
	self->crypted = 0;
	uuid_generate(self->iv);
	return 0;
}