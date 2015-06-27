#include <gtest/gtest.h>

#include "password_msgpack_persister.h"

#include "dummy_iv.h"

#define SETUP \
bg_password_msgpack_persister repository;\
bg_encryptor encryptor;\
bg_decryptor decryptor;\
bg_password_factory factory;\
bg_password_factory_init(&factory, &dummy_iv_init, NULL, &encryptor, &decryptor);\
bg_password_msgpack_persister_init(&repository, "shadow", &factory);

#define TEAR_DOWN \
repository.destroy(&repository)

TEST(bg_password_msgpack_persister, canAddPassword) {
	SETUP;
	bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);

	EXPECT_EQ(0, repository.number_passwords);

	repository.repository.add(&repository.repository, password);

	EXPECT_EQ(1, repository.number_passwords);
	TEAR_DOWN;
}

TEST(bg_password_msgpack_persister, canAdd2Passwords) {
	SETUP;
	bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);

	EXPECT_EQ(0, repository.number_passwords);

	repository.repository.add(&repository.repository, password);
	repository.repository.add(&repository.repository, password2);

	EXPECT_EQ(2, repository.number_passwords);
	TEAR_DOWN;
}

TEST(bg_password_msgpack_persister, passwordHasGoodPointerWhenAdded) {
	SETUP;
	bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);

	repository.repository.add(&repository.repository, password);

	EXPECT_EQ(password, repository.password_array[0]);
	TEAR_DOWN;
}

TEST(bg_password_msgpack_persister, passwordNotAddedTwiceWhenSavedMoreThanOnce) {
	SETUP;
	bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);

	repository.repository.add(&repository.repository, password);
	repository.repository.add(&repository.repository, password);
	repository.repository.add(&repository.repository, password);
	repository.repository.add(&repository.repository, password);

	EXPECT_EQ(1, repository.number_passwords);
	TEAR_DOWN;
}

#define SAVED_NAME_1 "myPassName"
#define SAVED_DESCRIPTION_1 "some password description"
static char SAVED_UUID_1[16], SAVED_IV_1[DUMMY_IV_LENGTH], SAVED_VALUE_1[32];

#define SAVED_NAME_2 "myPass2Name"
#define SAVED_DESCRIPTION_2 "some password2 description2"
static char SAVED_UUID_2[16], SAVED_IV_2[DUMMY_IV_LENGTH], SAVED_VALUE_2[32];

TEST(bg_password_msgpack_persister, canSerializeContainedPasswords) {
	SETUP;
	bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	memcpy(SAVED_UUID_1, password->uuid, 16);
	memcpy(SAVED_IV_1, password->iv.value, DUMMY_IV_LENGTH);
	strcat(password->name, SAVED_NAME_1);
	strcat(password->description, SAVED_DESCRIPTION_1);
	uuid_generate(*(uuid_t*)&SAVED_VALUE_1);
	uuid_generate(*(uuid_t*)&(SAVED_VALUE_1[16]));
	memcpy(password->value, SAVED_VALUE_1, 32);
	repository.repository.add(&repository.repository, password);

	remove("shadow");
	EXPECT_TRUE(access("shadow", F_OK) == -1 );
	repository.repository.persist(&repository.repository);
	EXPECT_TRUE(access("shadow", F_OK) == 0 );

	TEAR_DOWN;
}

TEST(bg_password_msgpack_persister, canDeserializeContainedPasswords) {
	SETUP;
	bg_password_msgpack_persister repository2;
	bg_password_factory factory2;
	bg_password_msgpack_persister_init(&repository2, "shadow", &factory2);
	bg_password_factory_init(&factory2, &dummy_iv_init, &repository2.repository, &encryptor, &decryptor);

	EXPECT_EQ(0, repository2.number_passwords);
	EXPECT_EQ(0, repository2.repository.load(&repository2.repository));
	EXPECT_EQ(1, repository2.number_passwords);

	repository2.destroy(&repository2);
	TEAR_DOWN;
}

TEST(bg_password_msgpack_persister, deserializationIsDeserializingGoodValue) {
	SETUP;
	bg_password_msgpack_persister repository2;
	bg_password_factory factory2;
	bg_password_msgpack_persister_init(&repository2, "shadow", &factory2);
	bg_password_factory_init(&factory2, &dummy_iv_init, &repository2.repository, &encryptor, &decryptor);

	EXPECT_EQ(0, repository2.repository.load(&repository2.repository));

	EXPECT_EQ(0, memcmp(SAVED_UUID_1, repository2.password_array[0]->uuid, 16));
	EXPECT_EQ(0, memcmp(SAVED_IV_1, repository2.password_array[0]->iv.value, DUMMY_IV_LENGTH));
	EXPECT_EQ(0, strcmp(SAVED_NAME_1, repository2.password_array[0]->name));
	EXPECT_EQ(0, strcmp(SAVED_DESCRIPTION_1, repository2.password_array[0]->description));
	char passvalue[BLURGATHER_PWD_MAX_VALUE_LEN];
	memset(passvalue, 0, BLURGATHER_PWD_MAX_VALUE_LEN);
	memcpy(passvalue, SAVED_VALUE_1, 32);
	EXPECT_EQ(0, memcmp(passvalue, repository2.password_array[0]->value, BLURGATHER_PWD_MAX_VALUE_LEN));

	repository2.destroy(&repository2);
	TEAR_DOWN;
}

TEST(bg_password_msgpack_persister, canSerialize2ContainedPasswords) {
	SETUP;

	bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	memcpy(SAVED_UUID_1, password->uuid, 16);
	memcpy(SAVED_IV_1, password->iv.value, DUMMY_IV_LENGTH);
	strcat(password->name, SAVED_NAME_1);
	strcat(password->description, SAVED_DESCRIPTION_1);
	uuid_generate(*(uuid_t*)&SAVED_VALUE_1);
	uuid_generate(*(uuid_t*)&(SAVED_VALUE_1[16]));
	memcpy(password->value, SAVED_VALUE_1, 32);
	repository.repository.add(&repository.repository, password);

	bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	memcpy(SAVED_UUID_2, password2->uuid, 16);
	memcpy(SAVED_IV_2, password2->iv.value, DUMMY_IV_LENGTH);
	strcat(password2->name, SAVED_NAME_2);
	strcat(password2->description, SAVED_DESCRIPTION_2);
	uuid_generate(*(uuid_t*)&SAVED_VALUE_2);
	uuid_generate(*(uuid_t*)&(SAVED_VALUE_2[16]));
	memcpy(password2->value, SAVED_VALUE_2, 32);
	repository.repository.add(&repository.repository, password2);

	remove("shadow");
	EXPECT_TRUE(access("shadow", F_OK) == -1 );
	repository.repository.persist(&repository.repository);
	EXPECT_TRUE(access("shadow", F_OK) == 0 );

	TEAR_DOWN;
}

TEST(bg_password_msgpack_persister, canDeserializeContained2Passwords) {
	SETUP;
	bg_password_msgpack_persister repository2;
	bg_password_factory factory2;
	bg_password_msgpack_persister_init(&repository2, "shadow", &factory2);
	bg_password_factory_init(&factory2, &dummy_iv_init, &repository2.repository, &encryptor, &decryptor);

	EXPECT_EQ(0, repository2.number_passwords);
	EXPECT_EQ(0, repository2.repository.load(&repository2.repository));
	EXPECT_EQ(2, repository2.number_passwords);

	repository2.destroy(&repository2);
	TEAR_DOWN;
}

TEST(bg_password_msgpack_persister, deserializationIsDeserializingGoodValueFor2Passwords) {
	SETUP;
	bg_password_msgpack_persister repository2;
	bg_password_factory factory2;
	bg_password_msgpack_persister_init(&repository2, "shadow", &factory2);
	bg_password_factory_init(&factory2, &dummy_iv_init, &repository2.repository, &encryptor, &decryptor);

	EXPECT_EQ(0, repository2.repository.load(&repository2.repository));

	EXPECT_EQ(0, memcmp(SAVED_UUID_1, repository2.password_array[0]->uuid, 16));
	EXPECT_EQ(0, memcmp(SAVED_IV_1, repository2.password_array[0]->iv.value, DUMMY_IV_LENGTH));
	EXPECT_EQ(0, strcmp(SAVED_NAME_1, repository2.password_array[0]->name));
	EXPECT_EQ(0, strcmp(SAVED_DESCRIPTION_1, repository2.password_array[0]->description));
	char passvalue[BLURGATHER_PWD_MAX_VALUE_LEN];
	memset(passvalue, 0, BLURGATHER_PWD_MAX_VALUE_LEN);
	memcpy(passvalue, SAVED_VALUE_1, 32);
	EXPECT_EQ(0, memcmp(passvalue, repository2.password_array[0]->value, BLURGATHER_PWD_MAX_VALUE_LEN));

	EXPECT_EQ(0, memcmp(SAVED_UUID_2, repository2.password_array[1]->uuid, 16));
	EXPECT_EQ(0, memcmp(SAVED_IV_2, repository2.password_array[1]->iv.value, DUMMY_IV_LENGTH));
	EXPECT_EQ(0, strcmp(SAVED_NAME_2, repository2.password_array[1]->name));
	EXPECT_EQ(0, strcmp(SAVED_DESCRIPTION_2, repository2.password_array[1]->description));
	char passvalue2[BLURGATHER_PWD_MAX_VALUE_LEN];
	memset(passvalue2, 0, BLURGATHER_PWD_MAX_VALUE_LEN);
	memcpy(passvalue2, SAVED_VALUE_2, 32);
	EXPECT_EQ(0, memcmp(passvalue2, repository2.password_array[1]->value, BLURGATHER_PWD_MAX_VALUE_LEN));

	repository2.destroy(&repository2);
	TEAR_DOWN;
}

TEST(bg_password_msgpack_persister, canRemove1PasswordWhen1Stored) {
	SETUP;
	bg_password* password1 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password1);

	EXPECT_EQ(1, repository.number_passwords);
	EXPECT_EQ(0, repository.repository.remove(&repository.repository, password1->uuid));
	EXPECT_EQ(0, repository.number_passwords);
	TEAR_DOWN;
}

TEST(bg_password_msgpack_persister, canRemoveFirstPasswordWhen2Stored) {
	SETUP;
	bg_password* password1 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password1);
	bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password2);

	EXPECT_EQ(2, repository.number_passwords);
	EXPECT_EQ(password1, repository.password_array[0]);
	EXPECT_EQ(0, repository.repository.remove(&repository.repository, password1->uuid));
	EXPECT_EQ(1, repository.number_passwords);
	EXPECT_EQ(password2, repository.password_array[0]);
	TEAR_DOWN;
}

TEST(bg_password_msgpack_persister, canRemoveSecondPasswordWhen2Stored) {
	SETUP;
	bg_password* password1 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password1);
	bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password2);

	EXPECT_EQ(2, repository.number_passwords);
	EXPECT_EQ(password1, repository.password_array[0]);
	EXPECT_EQ(0, repository.repository.remove(&repository.repository, password2->uuid));
	EXPECT_EQ(1, repository.number_passwords);
	EXPECT_EQ(password1, repository.password_array[0]);
	TEAR_DOWN;
}

TEST(bg_password_msgpack_persister, canRemoveMiddlePasswordWhen3Stored) {
	SETUP;
	bg_password* password1 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password1);
	bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password2);
	bg_password* password3 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password3);

	EXPECT_EQ(3, repository.number_passwords);
	EXPECT_EQ(password2, repository.password_array[1]);
	EXPECT_EQ(0, repository.repository.remove(&repository.repository, password2->uuid));
	EXPECT_EQ(2, repository.number_passwords);
	EXPECT_EQ(password3, repository.password_array[1]);
	TEAR_DOWN;
}

TEST(bg_password_msgpack_persister, cannotRemovePasswordWhenInvalidUUID) {
	SETUP;
	bg_password* password1 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password1);
	bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password2);
	bg_password* password3 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);

	EXPECT_EQ(2, repository.number_passwords);
	EXPECT_EQ(-1, repository.repository.remove(&repository.repository, password3->uuid));
	EXPECT_EQ(2, repository.number_passwords);
	EXPECT_EQ(password2, repository.password_array[1]);
	bg_password_free(password3);
	TEAR_DOWN;
}

TEST(bg_password_msgpack_persister, canSortByNamesPasswordsWhen3Stored) {
	SETUP;
	bg_password* password1 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password1);
	strcat(password1->name, "passC");
	bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password2);
	strcat(password2->name, "passA");
	bg_password* password3 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password3);
	strcat(password3->name, "passB");

	repository.repository.sort(&repository.repository);

	EXPECT_EQ(password2, repository.password_array[0]);
	EXPECT_EQ(password3, repository.password_array[1]);
	EXPECT_EQ(password1, repository.password_array[2]);
	TEAR_DOWN;
}