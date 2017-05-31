#include <sweetgreen/sweetgreen.h>
#include "mocks.h"
#include "blurgather/mcrypt_cryptor.h"
#include "blurgather/utilities.h"


#define PLAIN_TEXT_PWD "i am a very secret password"
#define PLAIN_SECRET_KEY "some secret key"
#define PLAIN_IV "1234567891234567891234567891234"
#define BUFFER_SIZE 256


bg_cryptor_t *cryptor;
bg_secret_key_t *secret_key;
char buffer[BUFFER_SIZE];
size_t buffer_length;
bg_iv_t *iv;


sweetgreen_setup(cryptor) {
  cryptor = (bg_cryptor_t*)bg_mcrypt_cryptor();

  if(strlen(PLAIN_IV) + 1 != bg_cryptor_iv_length(cryptor)) {
    fprintf(stderr, "IV PASSED TO CRYPTOR MUST MATCH ITS ALGORITHM LENGHT");
    exit(1);
  }

  turnoff_debug();

  reset_context();
  bgctx_register_allocator(ctx, &mock_allocator);
  bgctx_register_cryptor(ctx, cryptor);

  memset(buffer, 0, BUFFER_SIZE);
  strcat(buffer, PLAIN_TEXT_PWD);
  buffer_length = strlen(buffer);

  iv = bg_iv_new(PLAIN_IV, strlen(PLAIN_IV) + 1);
  secret_key = bg_secret_key_new(PLAIN_SECRET_KEY, strlen(PLAIN_SECRET_KEY));

  memset(buffer, 0, BUFFER_SIZE);
  memcpy(buffer, PLAIN_TEXT_PWD, strlen(PLAIN_TEXT_PWD));
  buffer_length = strlen(PLAIN_TEXT_PWD) + 1;

  reset_debug();
}

sweetgreen_teardown(cryptor) {
  bg_secret_key_free(secret_key);
  bg_iv_free(iv);
}


sweetgreen_test_define(cryptor, cryptReturnsErrorWhenSecretKeyNull) {
  sweetgreen_expect_equal(-1, bg_cryptor_encrypt(cryptor, buffer, buffer_length, NULL, iv));
}

sweetgreen_test_define(cryptor, cryptReturnsErrorWhenIVNotSet) {
  sweetgreen_expect_equal(-2, bg_cryptor_encrypt(cryptor, buffer, buffer_length, secret_key, NULL));
}

sweetgreen_test_define(cryptor, decryptReturnsErrorWhenSecretKeyNotSet) {
  sweetgreen_expect_equal(-1, bg_cryptor_decrypt(cryptor, buffer, buffer_length, NULL, iv));
}

sweetgreen_test_define(cryptor, decryptReturnsErrorWhenIVNotSet) {
  sweetgreen_expect_equal(-2, bg_cryptor_decrypt(cryptor, buffer, buffer_length, secret_key, NULL));
}

sweetgreen_test_define(cryptor, cryptReturns0WhenOK) {
  sweetgreen_expect_zero(bg_cryptor_encrypt(cryptor, buffer, buffer_length, secret_key, iv));
}

sweetgreen_test_define(cryptor, bufferValueChangesWhenCallingCrypt) {
  bg_cryptor_encrypt(cryptor, buffer, buffer_length, secret_key, iv);

  sweetgreen_expect_not_equal_memory(PLAIN_TEXT_PWD, buffer, buffer_length);
}

sweetgreen_test_define(cryptor, bufferValueGetsBackToInitialValueWhenCallingDecrypt) {
  char buffer_copy[BUFFER_SIZE];
  memcpy(buffer_copy, buffer, BUFFER_SIZE);

  bg_cryptor_encrypt(cryptor, buffer, buffer_length, secret_key, iv);
  bg_cryptor_decrypt(cryptor, buffer, buffer_length, secret_key, iv);

  sweetgreen_expect_equal_memory(buffer_copy, buffer, BUFFER_SIZE);
}

sweetgreen_test_define(cryptor, generates_a_valid_random_iv) {
  bg_iv_t *output_iv;

  sweetgreen_expect_zero(bg_cryptor_generate_iv(cryptor, &output_iv));
  sweetgreen_expect_not_null(bg_iv_data(output_iv));
  sweetgreen_expect_equal(32, bg_iv_length(output_iv));
}
