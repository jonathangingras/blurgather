#include <stdio.h>
#include <mcrypt.h>
#include <blurgather/iv.h>
#include <blurgather/string.h>
#include <blurgather/secret_key.h>
#include <blurgather/cryptor.h>
#include <blurgather/urandom_iv.h>

static int check_args(const bg_secret_key_t *secret_key, const bg_iv_t *iv) {
  if(!secret_key) {
    return -1;
  }
  if(!iv) {
    return -2;
  }
  return 0;
}

int bg_mcrypt_aes256_encrypt(void *memory, size_t memlen, const bg_secret_key_t *secret_key, const bg_iv_t *iv) {
  int error_code = 0;

  if((error_code = check_args(secret_key, iv))) {
    return error_code;
  }

  MCRYPT td = mcrypt_module_open("rijndael-256", NULL, "cfb", NULL);

  mcrypt_generic_init(td,
                      (void *)bg_secret_key_data(secret_key),
                      bg_secret_key_length(secret_key),
                      (void *)bg_iv_data(iv));

  error_code = mcrypt_generic(td, memory, memlen);

  mcrypt_generic_deinit(td);
  mcrypt_module_close(td);

  return error_code;
}

int bg_mcrypt_aes256_decrypt(void *memory, size_t memlen, const bg_secret_key_t *secret_key, const bg_iv_t *iv) {
  int error_code = 0;

  if((error_code = check_args(secret_key, iv))) {
    return error_code;
  }

  MCRYPT td = mcrypt_module_open("rijndael-256", NULL, "cfb", NULL);

  mcrypt_generic_init(td,
                      (void *)bg_secret_key_data(secret_key),
                      bg_secret_key_length(secret_key),
                      (void *)bg_iv_data(iv));

  error_code = mdecrypt_generic(td, memory, memlen);

  mcrypt_generic_deinit(td);
  mcrypt_module_close(td);

  return error_code;
}

size_t bg_mcrypt_iv32_iv_length() {
  return 32;
}

int bg_mcrypt_iv32_generate_iv(bg_iv_t **output) {
  unsigned char buffer[32];
  int error_code = 0;
  if((error_code = bg_get_devurandom_iv(buffer, 32))) {
    return error_code;
  }
  bg_iv_t *iv = bg_iv_new(buffer, 32);
  if(iv) {
    *output = iv;
  } else {
    error_code = -2;
  }
  return error_code;
}

const static bg_cryptor_t bg_mcrypt_aes256 = {
  .encrypt = &bg_mcrypt_aes256_encrypt,
  .decrypt = &bg_mcrypt_aes256_decrypt,
  .iv_length = &bg_mcrypt_iv32_iv_length,
  .generate_iv = &bg_mcrypt_iv32_generate_iv
};

const bg_cryptor_t *bg_mcrypt_cryptor() {
  return &bg_mcrypt_aes256;
}
