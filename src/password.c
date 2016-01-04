#include "utilities.h"
#include "password.h"

struct bg_password {
	void (* destroy)(bg_password* self);

	IV_t iv;
	char* name; //max 100 bytes
	char* description; //max 500 bytes
	char* value; //max 256 bytes
	size_t value_length; //actual length

	int crypted;

	bg_encryptor* encryptor;
	bg_decryptor* decryptor;
	bg_password_repository* repository;
};

size_t bg_password_size() {
  return sizeof(bg_password);
}

bg_password* bg_password_init(bg_password* _self, IV_init_callback iv_initializer, bg_encryptor* encryptor,
                                         bg_decryptor* decryptor, bg_password_repository* repository) {
	bg_password* self = _self;
	if(!_self) {
		self = (bg_password*) malloc(sizeof(bg_password));
	}

	self->encryptor = encryptor;
	self->decryptor = decryptor;
	self->repository = repository;

	iv_initializer(&self->iv);
	self->name = (char*) calloc(BLURGATHER_PWD_MAX_NAME_LEN, sizeof(char));
	self->description = (char*) calloc(BLURGATHER_PWD_MAX_DESCRIPTION_LEN, sizeof(char));
	self->value = (char*) calloc(BLURGATHER_PWD_MAX_VALUE_LEN, sizeof(char));
	self->value_length = 0;
	self->crypted = 0;

	self->destroy = &bg_password_destroy;

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
	return bg_pwd_repository_add(self->repository, self);
}

const char *bg_password_name(bg_password *password) {
  return (const char *) password->name;
}

int bg_password_update_name(bg_password *password, const char *name) {
  if(strlen(name) > BLURGATHER_PWD_MAX_NAME_LEN - 1) {
    return -1;
  }

  memset(password->name, 0, BLURGATHER_PWD_MAX_NAME_LEN);
  strcpy(password->name, name);
  return 0;
}

int bg_password_crypted(bg_password *password) {
  return password->crypted;
}

const char *bg_password_description(bg_password *password) {
  return (const char *) password->description;
}

int bg_password_update_description(bg_password *password, const char *description) {
  if(strlen(description) > BLURGATHER_PWD_MAX_DESCRIPTION_LEN - 1) {
    return -1;
  }

  memset(password->description, 0, BLURGATHER_PWD_MAX_DESCRIPTION_LEN);
  strcpy(password->description, description);
  return 0;
}

const unsigned char *bg_password_iv_value(bg_password *password) {
  return password->iv.value;
}

size_t bg_password_iv_length(bg_password *password) {
  return password->iv.length;
}

const char *bg_password_value(bg_password *password) {
  return (const char *) password->value;
}

const unsigned char *bg_password_raw_value(bg_password *password) {
  return (const unsigned char *) password->value;
}

size_t bg_password_raw_value_length(bg_password *password) {
  return password->value_length;
}

int bg_password_update_value(bg_password *password, const char *value) {
  size_t len = strlen(value);

  if(len > BLURGATHER_PWD_MAX_VALUE_LEN - 1) {
    return -1;
  }
  
  password->crypted = 0;
  password->iv.randomize(&password->iv);

  memset(password->value, 0, BLURGATHER_PWD_MAX_VALUE_LEN);
  strncpy(password->value, value, len);
  password->value_length = len;

  return 0;
}

size_t bg_password_value_length(bg_password *password) {
  return password->value_length;
}

int bg_password_fill_raw(bg_password *password, const IV_t *iv, const void* crypted_value, size_t crypted_value_size) {
  if(crypted_value_size < 1 || crypted_value_size > BLURGATHER_PWD_MAX_VALUE_LEN) {
    return -3;
  }
  if(password->value_length) {
    return -1;
  }
  
  if(bg_iv_copy(&password->iv, iv)) {
    return -2;
  }

  memset(password->value, 0, BLURGATHER_PWD_MAX_VALUE_LEN);
  memcpy(password->value, crypted_value, crypted_value_size);
  password->value_length = crypted_value_size;
  password->crypted = 1;
  
  return 0;
}
