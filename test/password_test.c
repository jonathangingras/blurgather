#include <sweetgreen/sweetgreen.h>

#include "password.h"
#include "encryptor.h"
#include "decryptor.h"


typedef struct {
	bg_encryptor encryptor;
	int cryptCalled, set_ivCalled;
} dummy_encryptor;

int dummy_encryptor_crypt(bg_encryptor *self, void *memory, size_t memlen, size_t max_length, size_t *writelen) {
	dummy_encryptor* actual_self = (dummy_encryptor*) self->object;
	memset(memory, 0, 255);
	memcpy(memory, "i was crypted", strlen("i was crypted") + 1);
	*writelen = strlen("i was crypted");
	actual_self->cryptCalled = 1;
	return 0;
}

void dummy_encryptor_set_iv(bg_encryptor* self, IV_t* iv) {
	dummy_encryptor* actual_self = (dummy_encryptor*) self->object;
	actual_self->set_ivCalled = 1;
}

void init_dummy_encryptor(dummy_encryptor* dummy) {
	dummy->encryptor.object = (void*) dummy;
	dummy->encryptor.crypt = &dummy_encryptor_crypt;
	dummy->encryptor.set_iv = &dummy_encryptor_set_iv;
	dummy->cryptCalled = 0;
	dummy->set_ivCalled = 0;
}


typedef struct {
	bg_decryptor decryptor;
	int decryptCalled, set_ivCalled;
} dummy_decryptor;

int dummy_decryptor_decrypt(bg_decryptor* self, void *memory, size_t memlen, size_t max_length, size_t *writelen) {
	dummy_decryptor* actual_self = (dummy_decryptor*) self->object;
	memset(memory, 0, 255);
	memcpy(memory, "i was decrypted", strlen("i was decrypted") + 1);
	*writelen = strlen("i was decrypted");
	actual_self->decryptCalled = 1;
	return 0;
}

void dummy_decryptor_set_iv(bg_decryptor* self, IV_t* iv) {
	dummy_decryptor* actual_self = (dummy_decryptor*) self->object;
	actual_self->set_ivCalled = 1;
}

void init_dummy_decryptor(dummy_decryptor* dummy) {
	dummy->decryptor.object = (void*) dummy;
	dummy->decryptor.decrypt = &dummy_decryptor_decrypt;
	dummy->decryptor.set_iv = &dummy_decryptor_set_iv;
	dummy->decryptCalled = 0;
	dummy->set_ivCalled = 0;
}

typedef struct {
	bg_password_repository repository;
	int addCalled;
} dummy_persister;

int dummy_persister_add(bg_password_repository* self, bg_password* password) {
	dummy_persister* actual_self = (dummy_persister*) self->object;
	actual_self->addCalled = 1;
	return 0;
}

struct bg_password_repository_vtable dummy_persister_vtable = {
  .add = &dummy_persister_add
};

void init_dummy_persister(dummy_persister* dummy) {
	dummy->repository.object = (void*) dummy;
	dummy->repository.vtable = &dummy_persister_vtable;
	dummy->addCalled = 0;
}


#include "dummy_iv.h"


dummy_encryptor encryptor;
dummy_decryptor decryptor;
dummy_persister repository;
bg_password *password;


sweetgreen_setup(password) {
  init_dummy_encryptor(&encryptor);
  init_dummy_decryptor(&decryptor);
  init_dummy_persister(&repository);

  password = bg_password_init(NULL, &dummy_iv_init, &encryptor.encryptor, &decryptor.decryptor, &repository.repository);
  
  randomizeCalled = 0;
}

sweetgreen_teardown(password) {
  bg_password_free(password);
  randomizeCalled = 0;
}

sweetgreen_test_define(password, cryptIsCalledWhenCalledFromPassword) {
  bg_password_crypt(password);

  sweetgreen_expect_true(encryptor.cryptCalled);
}

sweetgreen_test_define(password, cryptReturns0WhenOk) {
  sweetgreen_expect_zero(bg_password_crypt(password));
}

sweetgreen_test_define(password, cannotCryptWhenNULLEncryptor) {
  bg_password_destroy(password);
  bg_password_init(password, &dummy_iv_init, NULL, &decryptor.decryptor, &repository.repository);
  
  sweetgreen_expect_equal(-1, bg_password_crypt(password));
}

sweetgreen_test_define(password, cryptReturnsNon0WhenAlreadyCrypted) {
  bg_password_crypt(password);

  sweetgreen_expect_non_zero(bg_password_crypt(password));
}

sweetgreen_test_define(password, decryptReturns0WhenOk) {
  bg_password_crypt(password);
  
  sweetgreen_expect_zero(bg_password_decrypt(password));
}

sweetgreen_test_define(password, cannotDecryptWhenNULLDecryptor) {
  bg_password_destroy(password);
  bg_password_init(password, &dummy_iv_init, &encryptor.encryptor, NULL, &repository.repository);
  
  sweetgreen_expect_equal(-1, bg_password_decrypt(password));
}

sweetgreen_test_define(password, valueChangesWithValueSentByEncryptorWhenCallingCrypt) {
  bg_password_update_value(password, "hello");
  
  bg_password_crypt(password);
        
  sweetgreen_expect_equal_string("i was crypted", bg_password_value(password));
}

sweetgreen_test_define(password, setIVIsCalledFromEncryptorWhenCallingCrypt) {
  sweetgreen_expect_false(encryptor.set_ivCalled);
  
  bg_password_crypt(password);
  
  sweetgreen_expect_true(encryptor.set_ivCalled);
}

sweetgreen_test_define(password, valueChangesWithValueSentByDecryptorWhenCallingDecrypt) {
  bg_password_update_value(password, "veawfverwagarg");
  bg_password_crypt(password);
  
  bg_password_decrypt(password);

  sweetgreen_expect_equal(0, strcmp("i was decrypted", bg_password_value(password)));
}

sweetgreen_test_define(password, setIVIsCalledFromDecryptorWhenCallingDecrypt) {
  sweetgreen_expect_false(decryptor.set_ivCalled);
  bg_password_crypt(password);
  
  bg_password_decrypt(password);
        
  sweetgreen_expect_true(decryptor.set_ivCalled);
}

sweetgreen_test_define(password, addIsCalledWhenCallingSave) {
  repository.addCalled = 0;
  
  bg_password_save(password);
  
  sweetgreen_expect_true(repository.addCalled);
}

sweetgreen_test_define(password, addIsNotCalledWhenCallingSaveWhenNULLRepository) {
  repository.addCalled = 0;
  bg_password_destroy(password);
  bg_password_init(password, &dummy_iv_init, &encryptor.encryptor, &decryptor.decryptor, NULL);

  sweetgreen_expect_equal(-1, bg_password_save(password));

  sweetgreen_expect_false(repository.addCalled);
}

sweetgreen_test_define(password, valueChangesWithValueGivenToUpdateWhenUpdateCalled) {
  bg_password_update_value(password, "hey");
  sweetgreen_expect_equal(0, strcmp("hey", bg_password_value(password)));

  bg_password_update_value(password, "im new value");

  sweetgreen_expect_equal(0, strcmp("im new value", bg_password_value(password)));
}

sweetgreen_test_define(password, ivRandomizeCalledWhenUpdateCalled) {
  bg_password_update_value(password, "im new value");

  sweetgreen_expect_true(randomizeCalled);
}

sweetgreen_test_define(password, persistAintCalledWhenUpdateCalled) {
  bg_password_update_value(password, "im new value");

  sweetgreen_expect_false(repository.addCalled);
}

sweetgreen_test_define(password, cryptedBooleanIsSetToFalseWhenUpdateCalled) {	
  bg_password_crypt(password);

  bg_password_update_value(password, "im new value");

  sweetgreen_expect_false(bg_password_crypted(password));
}

sweetgreen_test_define(password, fill_rawSetsValueToGoodLengthAndMemory) {
  const char value[] = "SomeEncryptedPassword";
  const IV_t iv = {.value = (unsigned char *)"SoMeIvVaLuE000000000000", .length = 24};
  
  sweetgreen_expect_zero(bg_password_fill_raw(password, &iv, value, strlen(value)));

  sweetgreen_expect_equal_memory(value, bg_password_value(password), strlen(value));
  sweetgreen_expect_equal(strlen(value), bg_password_value_length(password));
}

sweetgreen_test_define(password, fill_rawSetsIVToGoodLengthAndMemory) {
  const char value[] = "SomeEncryptedPassword";
  const IV_t iv = {.value = (unsigned char *)"SoMeIvVaLuE000000000000", .length = 24};
  
  sweetgreen_expect_zero(bg_password_fill_raw(password, &iv, value, strlen(value)));

  sweetgreen_expect_equal_memory(iv.value, bg_password_iv_value(password), iv.length);
  sweetgreen_expect_equal(iv.length, bg_password_iv_length(password));
}
