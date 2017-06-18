#include <sweetgreen/sweetgreen.h>

#include <blurgather/password.h>
#include "mocks.h"

sweetgreen_setup(password) {
  turnoff_debug();

  reset_mock_secret_key();
  reset_mock_iv();
  reset_mock_cryptor();

  reset_debug();
}


sweetgreen_test_define(password, can_instantiate_password) {
  bg_password *pwd = bg_password_new();

  sweetgreen_expect_not_null(pwd);
}

sweetgreen_test_define(password, encrypt_is_called_when_called_from_password) {
  bg_password *pwd = bg_password_new();

  bg_password_crypt(pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_true(mock_encrypt_called);
}

sweetgreen_test_define(password, crypt_returns_0_when_ok) {
  bg_password *pwd = bg_password_new();

  sweetgreen_expect_zero(bg_password_crypt(pwd, &mock_cryptor, mock_secret_key));
}

sweetgreen_test_define(password, password_is_flagged_crypted_when_succesfully_crypted) {
  bg_password *pwd = bg_password_new();

  sweetgreen_expect_zero(bg_password_crypt(pwd, &mock_cryptor, mock_secret_key));
  sweetgreen_expect_true(bg_password_crypted(pwd));
}

sweetgreen_test_define(password, cryptor_encrypted_length_is_called_when_password_succesfully_crypted) {
  bg_password *pwd = bg_password_new();
  sweetgreen_expect_false(mock_encrypted_length_called);

  bg_password_crypt(pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_true(mock_encrypted_length_called);
}

sweetgreen_test_define(password, crypt_returns_non_zero_when_already_crypted) {
  bg_password *pwd = bg_password_new();

  bg_password_crypt(pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_non_zero(bg_password_crypt(pwd, &mock_cryptor, mock_secret_key));
}

sweetgreen_test_define(password, password_is_not_flagged_crypted_when_not_succesfully_crypted) {
  mock_encrypt_return_value = 1;
  bg_password *pwd = bg_password_new();

  sweetgreen_expect_non_zero(bg_password_crypt(pwd, &mock_cryptor, mock_secret_key));
  sweetgreen_expect_false(bg_password_crypted(pwd));
}

sweetgreen_test_define(password, password_is_not_flagged_crypted_when_null_secret_key) {
  mock_encrypt_return_value = 1;
  bg_password *pwd = bg_password_new();

  sweetgreen_expect_non_zero(bg_password_crypt(pwd, &mock_cryptor, NULL));
  sweetgreen_expect_false(bg_password_crypted(pwd));
}

sweetgreen_test_define(password, decrypt_is_called_when_called_from_password) {
  bg_password *pwd = bg_password_new();

  bg_password_crypt(pwd, &mock_cryptor, mock_secret_key);
  bg_password_decrypt(pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_true(mock_decrypt_called);
}

sweetgreen_test_define(password, decrypt_returns_0_when_ok) {
  bg_password *pwd = bg_password_new();

  bg_password_crypt(pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_zero(bg_password_decrypt(pwd, &mock_cryptor, mock_secret_key));
}

sweetgreen_test_define(password, cannot_encrypt_when_NULL_cryptor) {
  bg_password *pwd = bg_password_new();

  sweetgreen_expect_non_zero(bg_password_crypt(pwd, NULL, mock_secret_key));
}

sweetgreen_test_define(password, cannot_decrypt_when_NULL_cryptor) {
  bg_password *pwd = bg_password_new();

  bg_password_crypt(pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_non_zero(bg_password_decrypt(pwd, NULL, mock_secret_key));
}

sweetgreen_test_define(password, value_can_be_updated_when_empty) {
  bg_password *pwd = bg_password_new();

  char new_value_raw[] = "somepassvalue";
  bg_string *new_value = bg_string_from_char_array(new_value_raw, strlen(new_value_raw));
  bg_password_update_value(pwd, new_value, &mock_cryptor);

  sweetgreen_expect_equal_memory(new_value_raw, bg_string_data(bg_password_value(pwd)), strlen(new_value_raw));
}

sweetgreen_test_define(password, value_can_be_updated_when_non_empty) {
  bg_password *pwd = bg_password_new();

  char first_value_raw[] = "somepassvalue";
  bg_string *first_value = bg_string_from_char_array(first_value_raw, strlen(first_value_raw));
  bg_password_update_value(pwd, first_value, &mock_cryptor);

  char new_value_raw[] = "somesecondpassvalue";
  bg_string *new_value = bg_string_from_char_array(new_value_raw, strlen(new_value_raw));
  bg_password_update_value(pwd, new_value, &mock_cryptor);

  sweetgreen_expect_equal_memory(new_value_raw, bg_string_data(bg_password_value(pwd)), strlen(new_value_raw));
}

sweetgreen_test_define(password, cryptor_generate_iv_called_when_update_value) {
  bg_password *pwd = bg_password_new();

  char new_value_raw[] = "somepassvalue";
  bg_string *new_value = bg_string_from_char_array(new_value_raw, strlen(new_value_raw));
  bg_password_update_value(pwd, new_value, &mock_cryptor);

  sweetgreen_expect_true(mock_cryptor_generate_iv_called);
  sweetgreen_expect_equal_memory(bg_iv_data(mock_iv), bg_iv_data(bg_password_iv(pwd)), bg_iv_length(mock_iv));
}

sweetgreen_test_define(password, update_value_takes_memory_acquisition_of_string_passed) {
  bg_password *pwd = bg_password_new();

  char new_value_raw[] = "somepassvalue";
  bg_string *new_value = bg_string_from_char_array(new_value_raw, strlen(new_value_raw));
  bg_password_update_value(pwd, new_value, &mock_cryptor);

  sweetgreen_expect_same_address(new_value, bg_password_value(pwd));
}

sweetgreen_test_define(password, update_name_takes_memory_acquisition_of_string_passed) {
  bg_password *pwd = bg_password_new();

  char new_value_raw[] = "somepassname";
  bg_string *new_value = bg_string_from_char_array(new_value_raw, strlen(new_value_raw));
  bg_password_update_name(pwd, new_value);

  sweetgreen_expect_same_address(new_value, bg_password_name(pwd));
}

sweetgreen_test_define(password, update_description_takes_memory_acquisition_of_string_passed) {
  bg_password *pwd = bg_password_new();

  char new_value_raw[] = "somepassdesc";
  bg_string *new_value = bg_string_from_char_array(new_value_raw, strlen(new_value_raw));
  bg_password_update_description(pwd, new_value);

  sweetgreen_expect_same_address(new_value, bg_password_description(pwd));
}

sweetgreen_test_define(password, password_is_flagged_not_crypted_when_update_value) {
  bg_password *pwd = bg_password_new();

  bg_password_crypt(pwd, &mock_cryptor, mock_secret_key);
  sweetgreen_expect_true(bg_password_crypted(pwd));

  char new_value_raw[] = "somepassvalue";
  bg_string *new_value = bg_string_from_char_array(new_value_raw, strlen(new_value_raw));
  bg_password_update_value(pwd, new_value, &mock_cryptor);

  sweetgreen_expect_false(bg_password_crypted(pwd));
}

sweetgreen_test_define(password, value_is_set_correctly_when_fill_raw) {
  bg_password *pwd = bg_password_new();
  char crypted_value[] = "somecryptedvalue";

  sweetgreen_expect_zero(bg_password_fill_raw(pwd, mock_iv, crypted_value, strlen(crypted_value)));

  sweetgreen_expect_equal_memory(bg_string_data(bg_password_value(pwd)), crypted_value, strlen(crypted_value));
}

sweetgreen_test_define(password, iv_is_set_correctly_when_fill_raw) {
  bg_password *pwd = bg_password_new();
  char crypted_value[] = "somecryptedvalue";

  sweetgreen_expect_zero(bg_password_fill_raw(pwd, mock_iv, crypted_value, strlen(crypted_value)));

  sweetgreen_expect_equal_memory(bg_iv_data(bg_password_iv(pwd)), bg_iv_data(mock_iv), bg_iv_length(mock_iv));
}

sweetgreen_test_define(password, fill_raw_takes_memory_acquisition_of_the_iv_passed) {
  bg_password *pwd = bg_password_new();
  char crypted_value[] = "somecryptedvalue";

  sweetgreen_expect_zero(bg_password_fill_raw(pwd, mock_iv, crypted_value, strlen(crypted_value)));

  sweetgreen_expect_same_address(mock_iv, bg_password_iv(pwd));
}

sweetgreen_test_define(password, password_is_flagged_crypted_when_fill_raw) {
  bg_password *pwd = bg_password_new();
  char crypted_value[] = "somecryptedvalue";

  sweetgreen_expect_zero(bg_password_fill_raw(pwd, mock_iv, crypted_value, strlen(crypted_value)));

  sweetgreen_expect_true(bg_password_crypted(pwd));
}


int mock_decrypt_with_padding(void *memory,
                                     size_t memlen,
                                     const bg_secret_key_t *secret_key,
                                     const bg_iv_t *iv) {
  mock_decrypt_called = 1;
  char trailling_nuls[128];
  memset(trailling_nuls, 0, 128);
  memcpy(trailling_nuls, "hello", 5);
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

sweetgreen_test_define(password, value_is_nul_padded_using_needed_length_when_password_encrypted) {
  bg_password *pwd = bg_password_new();
  *(void**)&mock_cryptor.encrypted_length = &mock_encrypted_length_with_padding;
  *(void**)&mock_cryptor.encrypt = &mock_encrypt_with_padding;

  bg_password_crypt(pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_equal(128, bg_string_length(bg_password_value(pwd)));
}

sweetgreen_test_define(password, value_is_nul_stripped_when_password_decrypted) {
  bg_password *pwd = bg_password_new();
  *(void**)&mock_cryptor.encrypted_length = &mock_encrypted_length_with_padding;
  *(void**)&mock_cryptor.encrypt = &mock_encrypt_with_padding;
  *(void**)&mock_cryptor.decrypt = &mock_decrypt_with_padding;
  bg_password_crypt(pwd, &mock_cryptor, mock_secret_key);

  bg_password_decrypt(pwd, &mock_cryptor, mock_secret_key);

  sweetgreen_expect_equal(5, bg_string_length(bg_password_value(pwd)));
  sweetgreen_expect_equal_memory("hello", bg_string_data(bg_password_value(pwd)), 5);
}
