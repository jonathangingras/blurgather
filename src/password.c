#include <blurgather/password.h>
#include <blurgather/cryptor.h>
#include <blurgather/repository.h>
#include <blurgather/encryption.h>


struct bg_password {
  bg_string *name;
  bg_string *description;
  bg_string *value;

  int crypted;
};

size_t bg_password_size() {
  return sizeof(bg_password);
}

bg_password *bg_password_new(void) {
  bg_password *self = malloc(sizeof(bg_password));

  self->name = bg_string_new();
  self->description = bg_string_new();
  self->value = bg_string_new();

  self->crypted = 0;

  return self;
}

bg_password *bg_password_copy(const bg_password *password) {
  bg_password *self = malloc(sizeof(bg_password));

  self->name = bg_string_copy(password->name);
  self->description = bg_string_copy(password->description);
  self->value = bg_string_copy(password->value);
  self->crypted = password->crypted;

  return self;
}

void bg_password_free(bg_password *self) {
  bg_string_clean_free(self->name);
  bg_string_clean_free(self->description);
  bg_string_clean_free(self->value);
  free(self);
}


/* methods */

int bg_password_crypt(bg_password *self, bg_cryptor_t *cryptor, bg_secret_key_t *key) {
  int err = 0;
  if(self->crypted) {
    return -1;
  }
  if((err = bg_encrypt_string(&self->name, cryptor, key))) {
    return -2;
  }
  if((err = bg_encrypt_string(&self->description, cryptor, key))) {
    return -3;
  }
  if((err = bg_encrypt_string(&self->value, cryptor, key))) {
    return -4;
  }
  self->crypted = 1;
  return 0;
}

int bg_password_decrypt(bg_password *self, bg_cryptor_t *cryptor, bg_secret_key_t *key) {
  int err = 0;
  if(!self->crypted) {
    return -1;
  }
  if((err = bg_decrypt_string(&self->name, cryptor, key))) {
    return -2;
  }
  if((err = bg_decrypt_string(&self->description, cryptor, key))) {
    return -3;
  }
  if((err = bg_decrypt_string(&self->value, cryptor, key))) {
    return -4;
  }
  self->crypted = 0;
  return 0;
}

const bg_string *bg_password_name(bg_password *password) {
  return password->name;
}

int bg_password_crypted(bg_password *password) {
  return password->crypted;
}

const bg_string *bg_password_description(bg_password *password) {
  return password->description;
}

const bg_string *bg_password_value(bg_password *password) {
  return password->value;
}

static int check_str_non_empty(const bg_string *str) {
  return bg_string_length(str) == 0;
}

int bg_password_update_name(bg_password *password, bg_string *name) {
  int error_code = 0;
  if((error_code = check_str_non_empty(name))) { return error_code; }

  bg_string_clean_replace(password->name, name);

  return 0;
}

int bg_password_update_description(bg_password *password, bg_string *description) {
  int error_code = 0;
  if((error_code = check_str_non_empty(description))) { return error_code; }

  bg_string_clean_replace(password->description, description);

  return 0;
}

int bg_password_update_value(bg_password *password, bg_string *value) {
  int error_code = 0;
  if((error_code = check_str_non_empty(value))) { return error_code; }

  bg_string_clean_replace(password->value, value);

  return 0;
}

size_t bg_password_value_length(bg_password *password) {
  return bg_string_length(password->value);
}

int bg_password_fill_raw(bg_password *password, const void *crypted_value, size_t crypted_value_size) {
  if(bg_string_length(password->value)) {
    return -1;
  }

  bg_string_clean_free(password->value);
  password->value = bg_string_from_char_array(crypted_value, crypted_value_size);
  password->crypted = 1;

  return 0;
}
