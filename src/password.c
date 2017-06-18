#include <blurgather/password.h>
#include <blurgather/cryptor.h>
#include <blurgather/repository.h>

struct bg_password {
  bg_iv_t *iv;
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

  self->iv = NULL;
  self->name = bg_string_new();
  self->description = bg_string_new();
  self->value = bg_string_new();

  self->crypted = 0;

  return self;
}

void bg_password_free(bg_password *self) {
  if(self->iv) { bg_iv_free(self->iv); }
  bg_string_clean_free(self->name);
  bg_string_clean_free(self->description);
  bg_string_clean_free(self->value);
  free(self);
}


/* methods */

int bg_password_crypt(bg_password *self, bg_cryptor_t *cryptor, bg_secret_key_t *key) {
  if(!cryptor) { return -1; }
  if(self->crypted)  { return -2; }
  if(!key) { return -3; }

  size_t needed_length = bg_cryptor_encrypted_length(cryptor, bg_string_length(bg_password_value(self)));
  bg_string *buffer = bg_string_filled_with_length(0, needed_length);
  memcpy((void*)bg_string_data(buffer), bg_string_data(bg_password_value(self)), bg_string_length(bg_password_value(self)));

  int error_value = 0;
  if((error_value = bg_cryptor_encrypt(cryptor,
                                       (void *)bg_string_data(buffer),
                                       bg_string_length(buffer),
                                       key,
                                       bg_password_iv(self)))) {
    bg_string_clean_free(buffer);
    return error_value;
  }

  self->crypted = 1;
  bg_string_clean_replace(self->value, buffer);

  return 0;
}

int bg_password_decrypt(bg_password *self, bg_cryptor_t *cryptor, bg_secret_key_t *key) {
  if(!cryptor) { return -1; }
  if(!self->crypted) { return 1; }
  if(!key) { return -3; }

  bg_string *buffer = bg_string_copy(bg_password_value(self));

  int error_value = 0;
  if((error_value = bg_cryptor_decrypt(cryptor,
                                       (void *)bg_string_data(buffer),
                                       bg_string_length(buffer),
                                       key,
                                       bg_password_iv(self)))) {
    bg_string_clean_free(buffer);
    return error_value;
  }

  bg_string_strip_nuls(&buffer);

  self->crypted = 0;
  bg_string_clean_replace(self->value, buffer);

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

const bg_iv_t *bg_password_iv(bg_password *password) {
  return password->iv;
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

int bg_password_update_value(bg_password *password, bg_string *value, bg_cryptor_t *cryptor) {
  if(!cryptor) { return -1; }

  bg_cryptor_generate_iv(cryptor, &password->iv);
  bg_string_clean_replace(password->value, value);

  password->crypted = 0;

  return 0;
}

size_t bg_password_value_length(bg_password *password) {
  return bg_string_length(password->value);
}

int bg_password_fill_raw(bg_password *password, bg_iv_t *iv, const void *crypted_value, size_t crypted_value_size) {
  if(crypted_value_size < 1) {
    return -3;
  }
  if(bg_string_length(password->value)) {
    return -1;
  }
  if(!iv) {
    return -2;
  }

  *((bg_iv_t**)&password->iv) = (bg_iv_t*)iv;
  bg_string_clean_replace(password->value, bg_string_from_char_array(crypted_value, crypted_value_size));
  password->crypted = 1;

  return 0;
}
