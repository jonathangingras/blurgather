#include <sweetgreen/sweetgreen.h>

#include "blurgather/utilities.h"
#include "blurgather/mcrypt_cryptor.h"
#include "blurgather/password.h"
#include "blurgather/mcrypt_iv.h"
#include "mocks.h"


#define PLAIN_TEXT_PWD "i am a very secret password"
#define PLAIN_SECRET_KEY "some secret key"
#define PLAIN_IV "1234567891234567"
#define BUFFER_SIZE 256


bg_secret_key_t *secret_key;
char buffer[BUFFER_SIZE];
size_t buffer_length;
bg_iv_t *iv;


sweetgreen_setup(uninitialized_key_iv_cryptor) {
  memset(buffer, 0, BUFFER_SIZE);
  strcat(buffer, PLAIN_TEXT_PWD);
  buffer_length = strlen(buffer);
  iv = bg_iv_new(ctx, mock_iv_data, 32);
}

sweetgreen_teardown(uninitialized_key_iv_cryptor) {
  bg_iv_free(iv);
}

sweetgreen_setup(cryptor) {
  memset(buffer, 0, BUFFER_SIZE);
  strcat(buffer, PLAIN_TEXT_PWD);
  buffer_length = strlen(buffer);

  iv = bg_iv_new(ctx, mock_iv_data, 32);
  secret_key = bg_secret_key_new(ctx, PLAIN_SECRET_KEY, strlen(PLAIN_SECRET_KEY));
}

sweetgreen_teardown(cryptor) {
  bg_iv_free(iv);
}




sweetgreen_test_define(uninitialized_key_iv_cryptor, cryptReturnsErrorWhenSecretKeyNotSet) {
  sweetgreen_expect_equal(-1, cryptor.encryptor.crypt(&cryptor.encryptor, buffer, buffer_length, BUFFER_SIZE, &buffer_length));
}

sweetgreen_test_define(uninitialized_key_iv_cryptor, cryptReturnsErrorWhenIVNotSet) {
  secret_key = bg_secret_key_new(PLAIN_SECRET_KEY, strlen(PLAIN_SECRET_KEY));
  cryptor.encryptor.set_secret_key(&cryptor.encryptor, secret_key);

  sweetgreen_expect_equal(-2, cryptor.encryptor.crypt(&cryptor.encryptor, buffer, buffer_length, BUFFER_SIZE, &buffer_length));
}

sweetgreen_test_define(uninitialized_key_iv_cryptor, decryptReturnsErrorWhenSecretKeyNotSet) {
  sweetgreen_expect_equal(-1, cryptor.decryptor.decrypt(&cryptor.decryptor, buffer, buffer_length, BUFFER_SIZE, &buffer_length));
}

sweetgreen_test_define(uninitialized_key_iv_cryptor, decryptReturnsErrorWhenIVNotSet) {
  secret_key = bg_secret_key_new(PLAIN_SECRET_KEY, strlen(PLAIN_SECRET_KEY));
  cryptor.decryptor.set_secret_key(&cryptor.decryptor, secret_key);

  sweetgreen_expect_equal(-2, cryptor.decryptor.decrypt(&cryptor.decryptor, buffer, buffer_length, BUFFER_SIZE, &buffer_length));
}

sweetgreen_test_define(cryptor, settingSecretKeyAndIVEffective) {
  sweetgreen_expect_equal_memory(bg_secret_key_data(cryptor.secret_key), PLAIN_SECRET_KEY, strlen(PLAIN_SECRET_KEY));
  sweetgreen_expect_equal(strlen(PLAIN_SECRET_KEY), bg_secret_key_length(cryptor.secret_key));
  sweetgreen_expect_equal_memory(cryptor.iv->value, PLAIN_IV, 16);
}

sweetgreen_test_define(cryptor, cryptReturns0WhenOK) {
  sweetgreen_expect_zero(cryptor.encryptor.crypt(&cryptor.encryptor, buffer, buffer_length, BUFFER_SIZE, &buffer_length));
}

sweetgreen_test_define(cryptor, bufferValueChangesWhenCallingCrypt) {
  cryptor.encryptor.crypt(&cryptor.encryptor, buffer, buffer_length, BUFFER_SIZE, &buffer_length);

  sweetgreen_expect_not_equal_memory(PLAIN_TEXT_PWD, buffer, strlen(PLAIN_TEXT_PWD) + 1);
}

sweetgreen_test_define(cryptor, bufferLengthIsSetToGoodValueWhenCallingCrypt) {
  cryptor.encryptor.crypt(&cryptor.encryptor, buffer, buffer_length, BUFFER_SIZE, &buffer_length);

  size_t effective_buffer_length = bg_reverse_memlen((unsigned char*)buffer, BUFFER_SIZE);
  sweetgreen_expect_equal(effective_buffer_length, buffer_length);
}

sweetgreen_test_define(cryptor, bufferLengthIsSetToGoodValueWhenCallingDecrypt) {
  cryptor.encryptor.crypt(&cryptor.encryptor, buffer, buffer_length, BUFFER_SIZE, &buffer_length);

  cryptor.decryptor.decrypt(&cryptor.decryptor, buffer, buffer_length, BUFFER_SIZE, &buffer_length);

  sweetgreen_expect_equal(strlen(PLAIN_TEXT_PWD), buffer_length);
}

sweetgreen_test_define(cryptor, bufferValueGetsBackToInitialValueWhenCallingDecrypt) {
  char buffer_copy[BUFFER_SIZE];
  memcpy(buffer_copy, buffer, BUFFER_SIZE);

  cryptor.encryptor.crypt(&cryptor.encryptor, buffer, buffer_length, BUFFER_SIZE, &buffer_length);
  cryptor.decryptor.decrypt(&cryptor.decryptor, buffer, buffer_length, BUFFER_SIZE, &buffer_length);

  sweetgreen_expect_equal_memory(buffer_copy, buffer, BUFFER_SIZE);
}
