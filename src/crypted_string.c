#include <string.h>

#include <blurgather/crypted_string.h>


struct bg_crypted_string {
  bg_iv_t *iv;
  bg_string *str;
};


bg_string *bg_crypted_string_decrypt(const bg_crypted_string *cr_str, const bg_cryptor_t *cryptor, const bg_secret_key_t *key) {
  bg_string *str = NULL;
  size_t length = bg_string_length(cr_str->str);
  unsigned char *buffer = malloc(length);

  memcpy(buffer, bg_string_data(cr_str->str), length);

  if(!bg_cryptor_decrypt(cryptor, buffer, length, key, cr_str->iv)) {
    str = bg_string_from_char_array((char*)buffer, length);
  }

  memset(buffer, 0, length);
  free(buffer);
  return str;
}

bg_crypted_string *bg_crypted_string_new(const bg_string *str, const bg_cryptor_t *cryptor, const bg_secret_key_t *key) {
  bg_crypted_string *cr_str = NULL;
  size_t length = bg_string_length(str);
  unsigned char *buffer = malloc(length);
  bg_iv_t *iv;

  if(!bg_cryptor_generate_iv(cryptor, &iv)) {

    memcpy(buffer, bg_string_data(str), length);

    if(!bg_cryptor_encrypt(cryptor, buffer, length, key, iv)) {
      cr_str = malloc(sizeof(bg_crypted_string));
      cr_str->str = bg_string_from_char_array((char*)buffer, length);
      cr_str->iv = iv;
    } else {
      bg_iv_free(iv);
    }
  }

  memset(buffer, 0, length);
  free(buffer);
  return cr_str;
}
