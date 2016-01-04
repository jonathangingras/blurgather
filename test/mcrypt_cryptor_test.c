#include <sweetgreen/sweetgreen.h>

#include <kiki/utilities.h>
#include "mcrypt_cryptor.h"
#include "password.h"
#include "mcrypt_iv.h"

#define PLAIN_TEXT_PWD "i am a very secret password"
#define PLAIN_SECRET_KEY "some secret key"
#define PLAIN_IV "1234567891234567"

bg_mcrypt_cryptor cryptor;
bg_secret_key secret_key;
IV_t iv;
char buffer[BLURGATHER_PWD_MAX_VALUE_LEN];
size_t buffer_length;

sweetgreen_setup(uninitialized_key_iv_cryptor) {
  bg_mcrypt_cryptor_init(&cryptor);
  
  memset(buffer, 0, BLURGATHER_PWD_MAX_VALUE_LEN);
  strcat(buffer, PLAIN_TEXT_PWD);
  buffer_length = strlen(buffer);
}

sweetgreen_teardown(uninitialized_key_iv_cryptor) {
  cryptor.destroy(&cryptor);
}

sweetgreen_setup(cryptor) {
  bg_mcrypt_cryptor_init(&cryptor);
  
  memset(buffer, 0, BLURGATHER_PWD_MAX_VALUE_LEN);
  strcat(buffer, PLAIN_TEXT_PWD);
  buffer_length = strlen(buffer);
  
  bg_secret_key_init(&secret_key);
  secret_key.update(&secret_key, PLAIN_SECRET_KEY, strlen(PLAIN_SECRET_KEY) + 1);
  cryptor.encryptor.set_secret_key(&cryptor.encryptor, &secret_key);
  
  bg_mcrypt_iv_init(&iv);
  memcpy(iv.value, PLAIN_IV, strlen(PLAIN_IV));
  cryptor.encryptor.set_iv(&cryptor.encryptor, &iv);
}

sweetgreen_teardown(cryptor) {
  cryptor.destroy(&cryptor);

  iv.destroy(&iv);
}

sweetgreen_test_define(uninitialized_key_iv_cryptor, cryptReturnsErrorWhenSecretKeyNotSet) {
  sweetgreen_expect_equal(-1, cryptor.encryptor.crypt(&cryptor.encryptor, buffer, &buffer_length));
}

sweetgreen_test_define(uninitialized_key_iv_cryptor, cryptReturnsErrorWhenIVNotSet) {
  bg_secret_key_init(&secret_key);
  secret_key.update(&secret_key, PLAIN_SECRET_KEY, strlen(PLAIN_SECRET_KEY) + 1);
  cryptor.encryptor.set_secret_key(&cryptor.encryptor, &secret_key);

  sweetgreen_expect_equal(-2, cryptor.encryptor.crypt(&cryptor.encryptor, buffer, &buffer_length));
}

sweetgreen_test_define(uninitialized_key_iv_cryptor, decryptReturnsErrorWhenSecretKeyNotSet) {
  sweetgreen_expect_equal(-1, cryptor.decryptor.decrypt(&cryptor.decryptor, buffer, &buffer_length));
}

sweetgreen_test_define(uninitialized_key_iv_cryptor, decryptReturnsErrorWhenIVNotSet) {
  bg_secret_key_init(&secret_key);
  secret_key.update(&secret_key, PLAIN_SECRET_KEY, strlen(PLAIN_SECRET_KEY) + 1);
  cryptor.decryptor.set_secret_key(&cryptor.decryptor, &secret_key);

  sweetgreen_expect_equal(-2, cryptor.decryptor.decrypt(&cryptor.decryptor, buffer, &buffer_length));
}

sweetgreen_test_define(cryptor, settingSecretKeyAndIVEffective) {
  sweetgreen_expect_equal_memory(cryptor.secret_key->value, PLAIN_SECRET_KEY, strlen(PLAIN_SECRET_KEY));
  sweetgreen_expect_equal(strlen(PLAIN_SECRET_KEY) + 1, cryptor.secret_key->length);
  sweetgreen_expect_equal_memory(cryptor.iv->value, PLAIN_IV, 16);
}

sweetgreen_test_define(cryptor, cryptReturns0WhenOK) {
  sweetgreen_expect_zero(cryptor.encryptor.crypt(&cryptor.encryptor, buffer, &buffer_length));
}

sweetgreen_test_define(cryptor, bufferValueChangesWhenCallingCrypt) {
  cryptor.encryptor.crypt(&cryptor.encryptor, buffer, &buffer_length);

  sweetgreen_expect_not_equal_memory(PLAIN_TEXT_PWD, buffer, strlen(PLAIN_TEXT_PWD) + 1);
}

sweetgreen_test_define(cryptor, bufferLengthIsSetToGoodValueWhenCallingCrypt) {
  cryptor.encryptor.crypt(&cryptor.encryptor, buffer, &buffer_length);

  size_t effective_buffer_length = kiki_reverse_memlen((unsigned char*)buffer, BLURGATHER_PWD_MAX_VALUE_LEN);
  sweetgreen_expect_equal(effective_buffer_length, buffer_length);
}

sweetgreen_test_define(cryptor, bufferLengthIsSetToGoodValueWhenCallingDecrypt) {
  cryptor.encryptor.crypt(&cryptor.encryptor, buffer, &buffer_length);

  cryptor.decryptor.decrypt(&cryptor.decryptor, buffer, &buffer_length);

  sweetgreen_expect_equal(strlen(PLAIN_TEXT_PWD), buffer_length);
}

sweetgreen_test_define(cryptor, bufferValueGetsBackToInitialValueWhenCallingDecrypt) {
  char buffer_copy[BLURGATHER_PWD_MAX_VALUE_LEN];
  memcpy(buffer_copy, buffer, BLURGATHER_PWD_MAX_VALUE_LEN);

  cryptor.encryptor.crypt(&cryptor.encryptor, buffer, &buffer_length);
  cryptor.decryptor.decrypt(&cryptor.decryptor, buffer, &buffer_length);

  sweetgreen_expect_equal_memory(buffer_copy, buffer, BLURGATHER_PWD_MAX_VALUE_LEN);
}
