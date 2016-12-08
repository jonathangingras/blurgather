#include <blurgather/password.h>
#include <blurgather/cryptor.h>
#include <blurgather/repository.h>

struct bg_password {
  bg_iv_t *iv;
  bg_string *name;
  bg_string *description;
  bg_string *value;

  int crypted;

  bg_context *ctx;
};

size_t bg_password_size() {
  return sizeof(bg_password);
}

bg_password *bg_password_new(bg_context *ctx) {
  bg_cryptor_t *cryptor = bgctx_cryptor(ctx);
  if(!cryptor) { return NULL; }

  bg_password *self = bgctx_allocate(ctx, sizeof(bg_password));
  self->ctx = ctx;

  bg_cryptor_generate_iv(cryptor, &self->iv);

  self->name = bg_string_new();
  self->description = bg_string_new();
  self->value = bg_string_new();

  self->crypted = 0;

  return self;
}

void bg_password_free(bg_password *self) {
  bg_iv_free(self->iv);
  bg_string_free(self->name);
  bg_string_free(self->description);
  bg_string_free(self->value);
  free(self);
}


/* methods */

int bg_password_crypt(bg_password *self) {
  bg_cryptor_t *cryptor = bgctx_cryptor(self->ctx);
  if(!cryptor) { return -1; }
  if(self->crypted)  { return -2; }

  bg_string *buffer = bg_string_copy(self->value);

  int error_value = 0;
  if((error_value = bg_cryptor_encrypt(cryptor,
                                       (void *)bg_string_data(buffer),
                                       bg_string_length(buffer),
                                       bgctx_access_key(self->ctx),
                                       self->iv))) {
    bg_string_clean_free(buffer);
    return error_value;
  }

  self->crypted = 1;
  bg_string_clean_replace(self->value, buffer);

  return 0;
}

int bg_password_decrypt(bg_password *self) {
  bg_cryptor_t *cryptor = bgctx_cryptor(self->ctx);
  if(!cryptor) { return -1; }
  if(!self->crypted) { return 1; }

  bg_string *buffer = bg_string_copy(self->value);

  int error_value = 0;
  if((error_value = bg_cryptor_decrypt(cryptor,
                                       (void *)bg_string_data(buffer),
                                       bg_string_length(buffer),
                                       bgctx_access_key(self->ctx),
                                       self->iv))) {
    return 1;
  }

  self->crypted = 0;
  bg_string_clean_replace(self->value, buffer);

  return 0;
}

int bg_password_save(bg_password *self) {
  bg_repository_t *repo = bgctx_repository(self->ctx);
  if(!repo) { return -1; }
  return bg_repository_add(repo, self);
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

int bg_password_update_name(bg_password *password, const bg_string *name) {
  bg_string_clean_replace(password->name, bg_string_copy(name));

  return 0;
}

int bg_password_update_description(bg_password *password, const bg_string *description) {
  bg_string_clean_replace(password->name, bg_string_copy(description));

  return 0;
}

int bg_password_update_value(bg_password *password, const bg_string *value) {
  bg_cryptor_t *cryptor = bgctx_cryptor(password->ctx);
  if(!cryptor) { return -1; }

  bg_cryptor_generate_iv(cryptor, &password->iv);
  bg_string_clean_replace(password->value, bg_string_copy(value));

  return 0;
}

size_t bg_password_value_length(bg_password *password) {
  return bg_string_length(password->value);
}

bg_context *bg_password_ctx(bg_password *password) {
  return password->ctx;
}

int bg_password_fill_raw(bg_password *password, const bg_iv_t *iv, const void *crypted_value, size_t crypted_value_size) {
  if(crypted_value_size < 1) {
  return -3;
  }
  if(bg_string_length(password->value)) {
  return -1;
  }
  if(!iv || password->value) {
  return -2;
  }

  *((bg_iv_t**)&password->iv) = (bg_iv_t*)iv;
  bg_string_clean_replace(password->value, bg_string_from_char_array(crypted_value, crypted_value_size));
  password->crypted = 1;

  return 0;
}
