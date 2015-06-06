#include <gtest/gtest.h>

#include <kiki/utilities.h>
#include <kiki/pwd_mng/kiMCryptEncryptor.h>
#include <kiki/pwd_mng/kiPassword.h>
#include <kiki/pwd_mng/MCryptIV.h>

#define PLAIN_TEXT_PWD "i am a very secret password"

#define SETUP \
kiMCryptEncryptor cryptor;\
kiki_pwd_mng_kiMCryptEncryptor_init(&cryptor);\
char buffer[KIKI_PWD_MAX_VALUE_LEN];\
memset(buffer, 0, KIKI_PWD_MAX_VALUE_LEN);\
strcat(buffer, PLAIN_TEXT_PWD);\
size_t buffer_length = strlen(buffer);

#define SETUP_KEY_IV \
kiki_pwd_mng_secret_key_t secret_key;\
kiki_pwd_mng_secret_key_t_init(&secret_key);\
secret_key.update(&secret_key, lened_str("some secret key"));\
cryptor.encryptor.set_secret_key(&cryptor.encryptor, &secret_key);\
IV_t iv;\
kiki_pwd_mng_mcrypt_iv_init(&iv);\
memcpy(iv.value, lened_str("1234567891234567"));\
cryptor.encryptor.set_iv(&cryptor.encryptor, &iv);

#define TEAR_DOWN \
cryptor.destroy(&cryptor);

#define TEAR_DOWN_KEY_IV \
iv.destroy(&iv);


TEST(kiMCryptEncryptor, cryptReturnsErrorWhenSecretKeyNotSet) {
	SETUP;

	EXPECT_EQ(-1, cryptor.encryptor.crypt(&cryptor.encryptor, buffer, &buffer_length));
}

TEST(kiMCryptEncryptor, cryptReturnsErrorWhenIVNotSet) {
	SETUP;
	kiki_pwd_mng_secret_key_t secret_key;
	kiki_pwd_mng_secret_key_t_init(&secret_key);
	secret_key.update(&secret_key, lened_str("some secret key"));
	cryptor.encryptor.set_secret_key(&cryptor.encryptor, &secret_key);

	EXPECT_EQ(-2, cryptor.encryptor.crypt(&cryptor.encryptor, buffer, &buffer_length));
	TEAR_DOWN;
}

TEST(kiMCryptEncryptor, decryptReturnsErrorWhenSecretKeyNotSet) {
	SETUP;

	EXPECT_EQ(-1, cryptor.decryptor.decrypt(&cryptor.decryptor, buffer, &buffer_length));
	TEAR_DOWN;
}

TEST(kiMCryptEncryptor, decryptReturnsErrorWhenIVNotSet) {
	SETUP;
	kiki_pwd_mng_secret_key_t secret_key;
	kiki_pwd_mng_secret_key_t_init(&secret_key);
	secret_key.update(&secret_key, lened_str("some secret key"));
	cryptor.decryptor.set_secret_key(&cryptor.decryptor, &secret_key);

	EXPECT_EQ(-2, cryptor.decryptor.decrypt(&cryptor.decryptor, buffer, &buffer_length));
	TEAR_DOWN;
}

TEST(kiMCryptEncryptor, settingSecretKeyAndIVEffective) {
	SETUP;
	SETUP_KEY_IV;

	EXPECT_EQ(0, memcmp(cryptor.secret_key->value, lened_str("some secret key")));
	EXPECT_EQ(strlen("some secret key") + 1, cryptor.secret_key->length);
	EXPECT_EQ(0, memcmp(cryptor.iv->value, "1234567891234567", 16));
	TEAR_DOWN;
	TEAR_DOWN_KEY_IV;
}

TEST(kiMCryptEncryptor, cryptReturns0WhenOK) {
	SETUP;
	SETUP_KEY_IV;

	EXPECT_EQ(0, cryptor.encryptor.crypt(&cryptor.encryptor, buffer, &buffer_length));
	TEAR_DOWN;
	TEAR_DOWN_KEY_IV;
}

TEST(kiMCryptEncryptor, bufferValueChangesWhenCallingCrypt) {
	SETUP;
	SETUP_KEY_IV;

	cryptor.encryptor.crypt(&cryptor.encryptor, buffer, &buffer_length);

	EXPECT_NE(0, memcmp(PLAIN_TEXT_PWD, buffer, strlen(PLAIN_TEXT_PWD) + 1));
	TEAR_DOWN;
	TEAR_DOWN_KEY_IV;
}

TEST(kiMCryptEncryptor, bufferLengthIsSetToGoodValueWhenCallingCrypt) {
	SETUP;
	SETUP_KEY_IV;

	cryptor.encryptor.crypt(&cryptor.encryptor, buffer, &buffer_length);

	size_t effective_buffer_length = kiki_reverse_memlen((unsigned char*)buffer, KIKI_PWD_MAX_VALUE_LEN);
	EXPECT_EQ(effective_buffer_length, buffer_length);
	TEAR_DOWN;
	TEAR_DOWN_KEY_IV;
}

TEST(kiMCryptEncryptor, bufferLengthIsSetToGoodValueWhenCallingDecrypt) {
	SETUP;
	SETUP_KEY_IV;
	cryptor.encryptor.crypt(&cryptor.encryptor, buffer, &buffer_length);

	cryptor.decryptor.decrypt(&cryptor.decryptor, buffer, &buffer_length);

	EXPECT_EQ(strlen(PLAIN_TEXT_PWD), buffer_length);
	TEAR_DOWN;
	TEAR_DOWN_KEY_IV;
}

TEST(kiMCryptEncryptor, bufferValueGetsBackToInitialValueWhenCallingDecrypt) {
	SETUP;
	SETUP_KEY_IV;
	char buffer_copy[KIKI_PWD_MAX_VALUE_LEN];
	memcpy(buffer_copy, buffer, KIKI_PWD_MAX_VALUE_LEN);

	cryptor.encryptor.crypt(&cryptor.encryptor, buffer, &buffer_length);
	cryptor.decryptor.decrypt(&cryptor.decryptor, buffer, &buffer_length);

	EXPECT_EQ(0, memcmp(buffer_copy, buffer, KIKI_PWD_MAX_VALUE_LEN));
	TEAR_DOWN;
	TEAR_DOWN_KEY_IV;
}