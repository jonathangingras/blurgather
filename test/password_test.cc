#include <gtest/gtest.h>

#include "password.h"
#include "encryptor.h"
#include "decryptor.h"

extern "C" {

struct dummy_encryptor {
	bg_encryptor encryptor;
	int cryptCalled, set_ivCalled;
};

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


struct dummy_decryptor {
	bg_decryptor decryptor;
	int decryptCalled, set_ivCalled;
};

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

struct dummy_persister {
	bg_password_repository repository;
	int addCalled;
};

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

}

#include "dummy_iv.h"

#define SETUP \
dummy_encryptor encryptor; init_dummy_encryptor(&encryptor);\
dummy_decryptor decryptor; init_dummy_decryptor(&decryptor);\
dummy_persister repository; init_dummy_persister(&repository);\
bg_password password;\
bg_password_init(&password, &dummy_iv_init, &encryptor.encryptor, &decryptor.decryptor, &repository.repository);\
randomizeCalled = 0;

#define TEAR_DOWN \
password.destroy(&password);\
randomizeCalled = 0;

TEST(bg_password, cryptIsCalledWhenCalledFromPassword) {
	SETUP;

	EXPECT_FALSE(encryptor.cryptCalled);
	password.crypt(&password);
	EXPECT_NE(0, encryptor.cryptCalled);
	TEAR_DOWN;
}

TEST(bg_password, cryptReturns0WhenOk) {
	SETUP;

	EXPECT_FALSE(password.crypt(&password));
	TEAR_DOWN;
}

TEST(bg_password, cannotCryptWhenNULLEncryptor) {
	SETUP;
	password.encryptor = NULL;

	EXPECT_EQ(-1, password.crypt(&password));

	TEAR_DOWN;
}

TEST(bg_password, cryptReturnsNon0WhenNotOk) {
	SETUP;
	password.crypted = 1;

	EXPECT_TRUE(password.crypt(&password));
	TEAR_DOWN;
}

TEST(bg_password, decryptReturns0WhenOk) {
	SETUP;
	password.crypted = 1;

	EXPECT_FALSE(password.decrypt(&password));
	TEAR_DOWN;
}

TEST(bg_password, cannotDecryptWhenNULLDecryptor) {
	SETUP;
	password.decryptor = NULL;

	EXPECT_EQ(-1, password.decrypt(&password));
	TEAR_DOWN;
}

TEST(bg_password, valueChangesWithValueSentByEncryptorWhenCallingCrypt) {
	SETUP;
	password.update(&password, "hello", strlen("hello") + 1);
	password.crypted = 0;

	password.crypt(&password);

	EXPECT_EQ(0, strcmp("i was crypted", password.value));
	TEAR_DOWN;
}

TEST(bg_password, setIVIsCalledFromEncryptorWhenCallingCrypt) {
	SETUP;
	EXPECT_EQ(0, encryptor.set_ivCalled);

	password.crypt(&password);

	EXPECT_EQ(1, encryptor.set_ivCalled);
	TEAR_DOWN;
}

TEST(bg_password, valueChangesWithValueSentByDecryptorWhenCallingDecrypt) {
	SETUP;
	password.update(&password, "veawfverwagarg", strlen("veawfverwagarg") + 1);
	password.crypted = 1;

	password.decrypt(&password);

	EXPECT_EQ(0, strcmp("i was decrypted", password.value));
	TEAR_DOWN;
}

TEST(bg_password, setIVIsCalledFromDecryptorWhenCallingDecrypt) {
	SETUP;
	EXPECT_EQ(0, decryptor.set_ivCalled);
	password.crypted = 1;

	password.decrypt(&password);

	EXPECT_EQ(1, decryptor.set_ivCalled);
	TEAR_DOWN;
}

TEST(bg_password, addIsCalledWhenCallingSave) {
	SETUP;
	repository.addCalled = 0;

	password.save(&password);

	EXPECT_TRUE(repository.addCalled);
	TEAR_DOWN;
}


TEST(bg_password, addIsNotCalledWhenCallingSaveWhenNULLRepository) {
	SETUP;
	repository.addCalled = 0;
	password.repository = NULL;

	EXPECT_EQ(-1, password.save(&password));

	EXPECT_FALSE(repository.addCalled);
	TEAR_DOWN;
}

TEST(bg_password, valueChangesWithValueGivenToUpdateWhenUpdateCalled) {
	SETUP;
	strcat(password.value, "hey");
	EXPECT_EQ(0, strcmp("hey", password.value));

	password.update(&password, "im new value", strlen("im new value") + 1);

	EXPECT_EQ(0, strcmp("im new value", password.value));
	TEAR_DOWN;
}

TEST(bg_password, ivRandomizeCalledWhenUpdateCalled) {
	SETUP;

	password.update(&password, "im new value", strlen("im new value") + 1);

	EXPECT_TRUE(randomizeCalled);
	TEAR_DOWN;
}

TEST(bg_password, uuidDoesntChangesWhenUpdateCalled) {
	SETUP;
	unsigned char password_uuid_copy[16];
	memcpy(password_uuid_copy, password.uuid, 16);

	password.update(&password, "im new value", strlen("im new value") + 1);

	EXPECT_EQ(0, memcmp(password_uuid_copy, password.uuid, 16));
	TEAR_DOWN;
}

TEST(bg_password, persistAintCalledWhenUpdateCalled) {
	SETUP;

	password.update(&password, "im new value", strlen("im new value") + 1);

	EXPECT_FALSE(repository.addCalled);
	TEAR_DOWN;
}

TEST(bg_password, cryptedBooleanIsSetToFalseWhenUpdateCalled) {
	SETUP;
	password.crypt(&password);

	password.update(&password, "im new value", strlen("im new value") + 1);

	EXPECT_FALSE(password.crypted);
	TEAR_DOWN;
}