#include <gtest/gtest.h>

#include <kiki/pwd_mng/kiPasswordMCryptEncryptor.h>
#include <kiki/utilities.h>

#define PLAIN_TEXT_PWD "i am a password very very secret"

#define SETUP \
kiPasswordMCryptEncryptor cryptor;\
kiki_pwd_mng_kiPasswordMCryptEncryptor_init(&cryptor);\
char buffer[KIKI_PWD_MAX_VALUE_LEN];\
memset(buffer, 0, KIKI_PWD_MAX_VALUE_LEN);\
strcat(buffer, PLAIN_TEXT_PWD);

#define SETUP_KEY_IV \
kiki_pwd_mng_secret_key_t secret_key;\
kiki_pwd_mng_secret_key_t_init(&secret_key);\
secret_key.update(&secret_key, lened_str("some secret key"));\
cryptor.encryptor.set_secret_key(&cryptor.encryptor, secret_key);\
cryptor.encryptor.set_iv(&cryptor.encryptor, (IV_t)"1234567891234567");

#define TEAR_DOWN \
cryptor.destroy(&cryptor);


TEST(kiPasswordMCryptEncryptor, cryptReturnsErrorWhenSecretKeyNotSet) {
	SETUP;

	EXPECT_EQ(-1, cryptor.encryptor.crypt(&cryptor.encryptor, lened_str(buffer)));
}

TEST(kiPasswordMCryptEncryptor, cryptReturnsErrorWhenIVNotSet) {
	SETUP;
	kiki_pwd_mng_secret_key_t secret_key;
	kiki_pwd_mng_secret_key_t_init(&secret_key);
	secret_key.update(&secret_key, lened_str("some secret key"));
	cryptor.encryptor.set_secret_key(&cryptor.encryptor, secret_key);

	EXPECT_EQ(-2, cryptor.encryptor.crypt(&cryptor.encryptor, lened_str(buffer)));
	TEAR_DOWN;
}

TEST(kiPasswordMCryptEncryptor, decryptReturnsErrorWhenSecretKeyNotSet) {
	SETUP;

	EXPECT_EQ(-1, cryptor.decryptor.decrypt(&cryptor.decryptor, lened_str(buffer)));
	TEAR_DOWN;
}

TEST(kiPasswordMCryptEncryptor, decryptReturnsErrorWhenIVNotSet) {
	SETUP;
	kiki_pwd_mng_secret_key_t secret_key;
	kiki_pwd_mng_secret_key_t_init(&secret_key);
	secret_key.update(&secret_key, lened_str("some secret key"));
	cryptor.decryptor.set_secret_key(&cryptor.decryptor, secret_key);

	EXPECT_EQ(-2, cryptor.decryptor.decrypt(&cryptor.decryptor, lened_str(buffer)));
	TEAR_DOWN;
}

TEST(kiPasswordMCryptEncryptor, settingSecretKeyAndIVEffective) {
	SETUP;
	SETUP_KEY_IV;

	EXPECT_EQ(0, memcmp(cryptor.secret_key.value, lened_str("some secret key")));
	EXPECT_EQ(strlen("some secret key") + 1, cryptor.secret_key.length);
	EXPECT_EQ(0, memcmp(cryptor.iv, "1234567891234567", 16));
	TEAR_DOWN;
}

TEST(kiPasswordMCryptEncryptor, cryptReturns0WhenOK) {
	SETUP;
	SETUP_KEY_IV;

	EXPECT_EQ(0, cryptor.encryptor.crypt(&cryptor.encryptor, lened_str(buffer)));
	TEAR_DOWN;
}

TEST(kiPasswordMCryptEncryptor, bufferValueChangesWhenCallingCryptWhenIVAndKeySet) {
	SETUP;
	SETUP_KEY_IV;

	cryptor.encryptor.crypt(&cryptor.encryptor, lened_str(buffer));

	EXPECT_NE(0, memcmp(PLAIN_TEXT_PWD, buffer, strlen(PLAIN_TEXT_PWD) + 1));
	TEAR_DOWN;
}

TEST(kiPasswordMCryptEncryptor, bufferValueGetsBackToInitialValueWhenCallingDecrypt) {
	SETUP;
	SETUP_KEY_IV;
	char buffer_copy[KIKI_PWD_MAX_VALUE_LEN];
	memcpy(buffer_copy, buffer, KIKI_PWD_MAX_VALUE_LEN);

	cryptor.encryptor.crypt(&cryptor.encryptor, lened_str(buffer));
	cryptor.decryptor.decrypt(&cryptor.decryptor, buffer, kiki_reverse_memlen((unsigned char*)buffer, KIKI_PWD_MAX_VALUE_LEN));

	EXPECT_EQ(0, memcmp(buffer_copy, buffer, KIKI_PWD_MAX_VALUE_LEN));
	TEAR_DOWN;
}