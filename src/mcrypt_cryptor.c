#include <stdio.h>
#include <mcrypt.h>
#include <blurgather/iv.h>
#include <blurgather/string.h>
#include <blurgather/secret_key.h>
#include <blurgather/cryptor.h>
#include <blurgather/urandom_iv.h>

int bg_mcrypt_aes256_encrypt(void *memory, size_t memlen, const bg_secret_key_t *secret_key, const bg_iv_t *iv) {
  int error_code = 0;
  MCRYPT td = mcrypt_module_open("rijndael-256", NULL, "cfb", NULL);

  mcrypt_generic_init(td,
                      (void*)bg_secret_key_data(secret_key),
                      bg_secret_key_length(secret_key),
                      (void *)bg_iv_data(iv));

  error_code = mcrypt_generic(td, memory, memlen);

  mcrypt_generic_deinit(td);
  mcrypt_module_close(td);

  return error_code;
}

int bg_mcrypt_aes256_decrypt(void *memory, size_t memlen, const bg_secret_key_t *secret_key, const bg_iv_t *iv) {
  int error_code = 0;
  MCRYPT td = mcrypt_module_open("rijndael-256", NULL, "cfb", NULL);

  mcrypt_generic_init(td,
                      (void*)bg_secret_key_data(secret_key),
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
  return bg_get_devurandom_iv((unsigned char *)bg_iv_data(*output), 32);
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
