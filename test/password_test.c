#include <sweetgreen/sweetgreen.h>

#include "password.h"
#include "encryptor.h"
#include "decryptor.h"


typedef struct {
	bg_encryptor encryptor;
	int cryptCalled, set_ivCalled;
} dummy_encryptor;

int dummy_encryptor_crypt(bg_encryptor* self, void* memory, size_t* length) {
	dummy_encryptor* actual_self = (dummy_encryptor*) self->object;
	memset(memory, 0, 255);
	memcpy(memory, "i was crypted", strlen("i was crypted") + 1);
	*length = strlen("i was crypted");
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

int dummy_decryptor_decrypt(bg_decryptor* self, void* memory, size_t* length) {
	dummy_decryptor* actual_self = (dummy_decryptor*) self->object;
	memset(memory, 0, 255);
	memcpy(memory, "i was decrypted", strlen("i was decrypted") + 1);
	*length = strlen("i was decrypted");
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

void init_dummy_persister(dummy_persister* dummy) {
	dummy->repository.object = (void*) dummy;
	dummy->repository.add = &dummy_persister_add;
	dummy->addCalled = 0;
}


#include "dummy_iv.h"


dummy_encryptor encryptor;
dummy_decryptor decryptor;
dummy_persister repository;
bg_password password;


sweetgreen_setup(password) {
  init_dummy_encryptor(&encryptor);
  init_dummy_decryptor(&decryptor);
  init_dummy_persister(&repository);

  bg_password_init(&password, &dummy_iv_init, &encryptor.encryptor, &decryptor.decryptor, &repository.repository);
  
  randomizeCalled = 0;
}

sweetgreen_teardown(password) {
  password.destroy(&password);
  randomizeCalled = 0;
}

sweetgreen_test_define(password, cryptIsCalledWhenCalledFromPassword) {
  password.crypt(&password);

  sweetgreen_expect_true(encryptor.cryptCalled);
}

sweetgreen_test_define(password, cryptReturns0WhenOk) {
  sweetgreen_expect_zero(password.crypt(&password));
}

sweetgreen_test_define(password, cannotCryptWhenNULLEncryptor) {
  password.encryptor = NULL;
  
  sweetgreen_expect_equal(-1, password.crypt(&password));
}

sweetgreen_test_define(password, cryptReturnsNon0WhenNotOk) {
  password.crypted = 1;

  sweetgreen_expect_non_zero(password.crypt(&password));
}

sweetgreen_test_define(password, decryptReturns0WhenOk) {
  password.crypted = 1;
  
  sweetgreen_expect_zero(password.decrypt(&password));
}

sweetgreen_test_define(password, cannotDecryptWhenNULLDecryptor) {
  password.decryptor = NULL;
  
  sweetgreen_expect_equal(-1, password.decrypt(&password));
}

sweetgreen_test_define(password, valueChangesWithValueSentByEncryptorWhenCallingCrypt) {
  password.update(&password, "hello", strlen("hello") + 1);
  password.crypted = 0;
  
  password.crypt(&password);
        
  sweetgreen_expect_equal(0, strcmp("i was crypted", password.value));
}

sweetgreen_test_define(password, setIVIsCalledFromEncryptorWhenCallingCrypt) {
  sweetgreen_expect_false(encryptor.set_ivCalled);
  
  password.crypt(&password);
  
  sweetgreen_expect_true(encryptor.set_ivCalled);
}

sweetgreen_test_define(password, valueChangesWithValueSentByDecryptorWhenCallingDecrypt) {
  password.update(&password, "veawfverwagarg", strlen("veawfverwagarg") + 1);
  password.crypted = 1;
  
  password.decrypt(&password);

  sweetgreen_expect_equal(0, strcmp("i was decrypted", password.value));
}

sweetgreen_test_define(password, setIVIsCalledFromDecryptorWhenCallingDecrypt) {
  sweetgreen_expect_false(decryptor.set_ivCalled);
  password.crypted = 1;
  
  password.decrypt(&password);
        
  sweetgreen_expect_true(decryptor.set_ivCalled);
}

sweetgreen_test_define(password, addIsCalledWhenCallingSave) {
  repository.addCalled = 0;
  
  password.save(&password);
  
  sweetgreen_expect_true(repository.addCalled);
}

sweetgreen_test_define(password, addIsNotCalledWhenCallingSaveWhenNULLRepository) {
  repository.addCalled = 0;
  password.repository = NULL;

  sweetgreen_expect_equal(-1, password.save(&password));

  sweetgreen_expect_false(repository.addCalled);
}

sweetgreen_test_define(password, valueChangesWithValueGivenToUpdateWhenUpdateCalled) {
  strcat(password.value, "hey");
  sweetgreen_expect_equal(0, strcmp("hey", password.value));

  password.update(&password, "im new value", strlen("im new value") + 1);

  sweetgreen_expect_equal(0, strcmp("im new value", password.value));
}

sweetgreen_test_define(password, ivRandomizeCalledWhenUpdateCalled) {
  password.update(&password, "im new value", strlen("im new value") + 1);

  sweetgreen_expect_true(randomizeCalled);
}

sweetgreen_test_define(password, persistAintCalledWhenUpdateCalled) {
  password.update(&password, "im new value", strlen("im new value") + 1);

  sweetgreen_expect_false(repository.addCalled);
}

sweetgreen_test_define(password, cryptedBooleanIsSetToFalseWhenUpdateCalled) {	
  password.crypt(&password);

  password.update(&password, "im new value", strlen("im new value") + 1);

  sweetgreen_expect_false(password.crypted);
}
