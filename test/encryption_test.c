#include <sweetgreen/sweetgreen.h>

#include <blurgather/string.h>
#include <blurgather/encryption.h>
#include "mocks.h"

sweetgreen_setup(encryption) {
  turnoff_debug();

  reset_mock_secret_key();
  reset_mock_iv();
  reset_mock_cryptor();

  reset_debug();
}

#define VALID_STR "hello"


sweetgreen_test_define(encryption, mock_encrypt_is_called_when_called_from_encrypt) {
  bg_string *pwd = bg_string_from_str(VALID_STR);

  bg_encrypt_string(&pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_true(mock_encrypt_called);
}

sweetgreen_test_define(encryption, crypt_returns_0_when_ok) {
  bg_string *pwd = bg_string_from_str(VALID_STR);

  sweetgreen_expect_zero(bg_encrypt_string(&pwd, &mock_cryptor, mock_secret_key));
}

sweetgreen_test_define(encryption, cryptor_encrypted_length_is_called_when_string_succesfully_crypted) {
  bg_string *pwd = bg_string_from_str(VALID_STR);
  sweetgreen_expect_false(mock_encrypted_length_called);

  bg_encrypt_string(&pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_true(mock_encrypted_length_called);
}

sweetgreen_test_define(encryption, mock_decrypt_is_called_when_called_from_decrypt) {
  bg_string *pwd = bg_string_from_str(VALID_STR);

  bg_encrypt_string(&pwd, &mock_cryptor, mock_secret_key);
  bg_decrypt_string(&pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_true(mock_decrypt_called);
}

sweetgreen_test_define(encryption, decrypt_returns_0_when_ok) {
  bg_string *pwd = bg_string_from_str(VALID_STR);

  bg_encrypt_string(&pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_zero(bg_decrypt_string(&pwd, &mock_cryptor, mock_secret_key));
}

sweetgreen_test_define(encryption, cannot_encrypt_when_NULL_cryptor) {
  bg_string *pwd = bg_string_from_str(VALID_STR);

  sweetgreen_expect_non_zero(bg_encrypt_string(&pwd, NULL, mock_secret_key));
}

sweetgreen_test_define(encryption, cannot_decrypt_when_NULL_cryptor) {
  bg_string *pwd = bg_string_from_str(VALID_STR);

  bg_encrypt_string(&pwd, NULL, mock_secret_key);

  sweetgreen_expect_non_zero(bg_decrypt_string(&pwd, NULL, mock_secret_key));
}

sweetgreen_test_define(encryption, cryptor_generate_iv_called_when_encrypt) {
  bg_string *pwd = bg_string_from_str(VALID_STR);

  bg_encrypt_string(&pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_true(mock_cryptor_generate_iv_called);
}

sweetgreen_test_define(encryption, iv_is_put_first_when_encrypted) {
  bg_string *pwd = bg_string_from_str(VALID_STR);

  bg_encrypt_string(&pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_equal_memory(bg_iv_data(mock_iv), bg_string_data(pwd), bg_iv_length(mock_iv));
}

sweetgreen_test_define(encryption, cipher_is_put_second_when_encrypted) {
  bg_string *pwd = bg_string_from_str(VALID_STR);

  bg_encrypt_string(&pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_equal_memory(VALID_STR, bg_string_data(pwd) + bg_iv_length(mock_iv), 6);
}

sweetgreen_test_define(encryption, iv_plus_cipher_length_is_ok_when_encrypted) {
  bg_string *pwd = bg_string_from_str(VALID_STR);

  bg_encrypt_string(&pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_equal(strlen(VALID_STR) + bg_iv_length(mock_iv), bg_string_length(pwd));
}

sweetgreen_test_define(encryption, string_is_correctly_separated_when_decrypt) {
  bg_string *pwd = bg_string_from_str(VALID_STR);

  bg_encrypt_string(&pwd, &mock_cryptor, mock_secret_key);
  bg_decrypt_string(&pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_equal(strlen(VALID_STR), bg_string_length(pwd));
  sweetgreen_expect_equal_string(VALID_STR, bg_string_data(pwd));
}

int mock_decrypt_with_padding(void *memory,
                                     size_t memlen,
                                     const bg_secret_key_t *secret_key,
                                     const bg_iv_t *iv) {
  mock_decrypt_called = 1;
  char trailling_nuls[128];
  memset(trailling_nuls, 0, 128);
  memcpy(trailling_nuls, VALID_STR, strlen(VALID_STR));
  memcpy(memory, trailling_nuls, 128);

  return mock_decrypt_return_value;
}

int mock_encrypt_with_padding(void *memory,
                 size_t memlen,
                 const bg_secret_key_t *secret_key,
                 const bg_iv_t *iv) {
  mock_encrypt_called = 1;

  return mock_encrypt_return_value;
}

size_t mock_encrypted_length_with_padding(size_t input_memlen) {
  mock_encrypted_length_called = 1;
  return 128;
}

sweetgreen_test_define(encryption, value_is_nul_padded_using_needed_length_when_password_encrypted) {
  bg_string *pwd = bg_string_from_str(VALID_STR);
  *(void**)&mock_cryptor.encrypted_length = &mock_encrypted_length_with_padding;
  *(void**)&mock_cryptor.encrypt = &mock_encrypt_with_padding;

  bg_encrypt_string(&pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_equal(bg_iv_length(mock_iv) + 128, bg_string_length(pwd));
}

sweetgreen_test_define(encryption, value_is_nul_stripped_when_password_decrypted) {
  bg_string *pwd = bg_string_from_str(VALID_STR);
  *(void**)&mock_cryptor.encrypted_length = &mock_encrypted_length_with_padding;
  *(void**)&mock_cryptor.encrypt = &mock_encrypt_with_padding;
  *(void**)&mock_cryptor.decrypt = &mock_decrypt_with_padding;
  bg_encrypt_string(&pwd, &mock_cryptor, mock_secret_key);

  bg_decrypt_string(&pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_equal(strlen(VALID_STR), bg_string_length(pwd));
  sweetgreen_expect_equal_memory(VALID_STR, bg_string_data(pwd), strlen(VALID_STR));
}
