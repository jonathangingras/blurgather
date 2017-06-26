#include <string.h>
#include <blurgather/string.h>
#include <blurgather/cryptor.h>


int bg_encrypt_string(bg_string **str, const bg_cryptor_t *cryptor, const bg_secret_key_t *key) {
  int err = 0;
  if(!cryptor) { return -1; }
  if(!key) { return -3; }

  bg_iv_t *iv;
  if((err = bg_cryptor_generate_iv(cryptor, &iv))) {
    return err;
  }

  size_t needed_length = bg_cryptor_encrypted_length(cryptor, bg_string_length(*str));
  bg_string *buffer = bg_string_filled_with_length(0, needed_length);
  memcpy((void*)bg_string_data(buffer), bg_string_data(*str), bg_string_length(*str));

  if((err = bg_cryptor_encrypt(cryptor,
                               (void *)bg_string_data(buffer),
                               bg_string_length(buffer),
                               key,
                               iv))) {
    bg_string_clean_free(buffer);
    return err;
  }

  bg_string_clean_free(*str);
  *str = bg_string_plus(bg_string_from_char_array(bg_iv_data(iv), bg_iv_length(iv)), buffer);

  bg_iv_free(iv);
  return 0;
}

int bg_decrypt_string(bg_string **str, const bg_cryptor_t *cryptor, const bg_secret_key_t *key) {
  int err = 0;
  if(!cryptor) { return -1; }
  if(!key) { return -3; }

  bg_iv_t *iv;
  bg_string *cr_str;
  {
    bg_string *lhs, *rhs;
    if((err = bg_string_split_after(*str, bg_cryptor_iv_length(cryptor), &lhs, &rhs))) {
      return err;
    }
    iv = bg_iv_new(bg_string_data(lhs), bg_string_length(lhs));
    bg_string_free(lhs);
    cr_str = rhs;
  }

  if((err = bg_cryptor_decrypt(cryptor,
                               (void *)bg_string_data(cr_str),
                               bg_string_length(cr_str),
                               key,
                               iv))) {
    bg_string_clean_free(cr_str);
    bg_iv_free(iv);
    return err;
  }
  bg_string_strip_nuls(&cr_str);

  bg_string_clean_free(*str);
  *str = cr_str;

  bg_iv_free(iv);
  return 0;
}
