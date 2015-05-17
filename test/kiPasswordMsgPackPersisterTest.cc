#include <gtest/gtest.h>

#include <kiki/pwd_mng/kiPasswordMsgPackPersister.h>

#define SETUP \
kiPasswordMsgPackPersister repository;\
kiEncryptor encryptor;\
kiDecryptor decryptor;\
kiPasswordFactory factory;\
kiki_pwd_mng_kiPasswordFactory_init(&factory, NULL, &encryptor, &decryptor);\
kiki_pwd_mng_kiPasswordMsgPackPersister_init(&repository, "shadow", &factory);

#define TEAR_DOWN \
repository.destroy(&repository)

TEST(kiPasswordMsgPackPersister, canAddPassword) {
	SETUP;
	kiPassword* password = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);

	EXPECT_EQ(0, repository.number_passwords);

	repository.repository.add(&repository.repository, password);

	EXPECT_EQ(1, repository.number_passwords);
	TEAR_DOWN;
}

TEST(kiPasswordMsgPackPersister, canAdd2Passwords) {
	SETUP;
	kiPassword* password = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	kiPassword* password2 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);

	EXPECT_EQ(0, repository.number_passwords);

	repository.repository.add(&repository.repository, password);
	repository.repository.add(&repository.repository, password2);

	EXPECT_EQ(2, repository.number_passwords);
	TEAR_DOWN;
}

TEST(kiPasswordMsgPackPersister, passwordHasGoodPointerWhenAdded) {
	SETUP;
	kiPassword* password = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);

	repository.repository.add(&repository.repository, password);

	EXPECT_EQ(password, repository.password_array[0]);
	TEAR_DOWN;
}

TEST(kiPasswordMsgPackPersister, passwordNotAddedTwiceWhenSavedMoreThanOnce) {
	SETUP;
	kiPassword* password = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);

	repository.repository.add(&repository.repository, password);
	repository.repository.add(&repository.repository, password);
	repository.repository.add(&repository.repository, password);
	repository.repository.add(&repository.repository, password);

	EXPECT_EQ(1, repository.number_passwords);
	TEAR_DOWN;
}

#define SAVED_NAME_1 "myPassName"
#define SAVED_DESCRIPTION_1 "some password description"
static char SAVED_UUID_1[16], SAVED_IV_1[16], SAVED_VALUE_1[32];

#define SAVED_NAME_2 "myPass2Name"
#define SAVED_DESCRIPTION_2 "some password2 description2"
static char SAVED_UUID_2[16], SAVED_IV_2[16], SAVED_VALUE_2[32];

TEST(kiPasswordMsgPackPersister, canSerializeContainedPasswords) {
	SETUP;
	kiPassword* password = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	memcpy(SAVED_UUID_1, password->uuid, 16);
	memcpy(SAVED_IV_1, password->iv, 16);
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

TEST(kiPasswordMsgPackPersister, canDeserializeContainedPasswords) {
	SETUP;
	kiPasswordMsgPackPersister repository2;
	kiPasswordFactory factory2;
	kiki_pwd_mng_kiPasswordMsgPackPersister_init(&repository2, "shadow", &factory2);
	kiki_pwd_mng_kiPasswordFactory_init(&factory2, &repository2.repository, &encryptor, &decryptor);

	EXPECT_EQ(0, repository2.number_passwords);
	EXPECT_EQ(0, repository2.repository.load(&repository2.repository));
	EXPECT_EQ(1, repository2.number_passwords);

	repository2.destroy(&repository2);
	TEAR_DOWN;
}

TEST(kiPasswordMsgPackPersister, deserializationIsDeserializingGoodValue) {
	SETUP;
	kiPasswordMsgPackPersister repository2;
	kiPasswordFactory factory2;
	kiki_pwd_mng_kiPasswordMsgPackPersister_init(&repository2, "shadow", &factory2);
	kiki_pwd_mng_kiPasswordFactory_init(&factory2, &repository2.repository, &encryptor, &decryptor);

	EXPECT_EQ(0, repository2.repository.load(&repository2.repository));

	EXPECT_EQ(0, memcmp(SAVED_UUID_1, repository2.password_array[0]->uuid, 16));
	EXPECT_EQ(0, memcmp(SAVED_IV_1, repository2.password_array[0]->iv, 16));
	EXPECT_EQ(0, strcmp(SAVED_NAME_1, repository2.password_array[0]->name));
	EXPECT_EQ(0, strcmp(SAVED_DESCRIPTION_1, repository2.password_array[0]->description));
	char passvalue[KIKI_PWD_MAX_VALUE_LEN];
	memset(passvalue, 0, KIKI_PWD_MAX_VALUE_LEN);
	memcpy(passvalue, SAVED_VALUE_1, 32);
	EXPECT_EQ(0, memcmp(passvalue, repository2.password_array[0]->value, KIKI_PWD_MAX_VALUE_LEN));

	repository2.destroy(&repository2);
	TEAR_DOWN;
}

TEST(kiPasswordMsgPackPersister, canSerialize2ContainedPasswords) {
	SETUP;

	kiPassword* password = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	memcpy(SAVED_UUID_1, password->uuid, 16);
	memcpy(SAVED_IV_1, password->iv, 16);
	strcat(password->name, SAVED_NAME_1);
	strcat(password->description, SAVED_DESCRIPTION_1);
	uuid_generate(*(uuid_t*)&SAVED_VALUE_1);
	uuid_generate(*(uuid_t*)&(SAVED_VALUE_1[16]));
	memcpy(password->value, SAVED_VALUE_1, 32);
	repository.repository.add(&repository.repository, password);

	kiPassword* password2 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	memcpy(SAVED_UUID_2, password2->uuid, 16);
	memcpy(SAVED_IV_2, password2->iv, 16);
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

TEST(kiPasswordMsgPackPersister, canDeserializeContained2Passwords) {
	SETUP;
	kiPasswordMsgPackPersister repository2;
	kiPasswordFactory factory2;
	kiki_pwd_mng_kiPasswordMsgPackPersister_init(&repository2, "shadow", &factory2);
	kiki_pwd_mng_kiPasswordFactory_init(&factory2, &repository2.repository, &encryptor, &decryptor);

	EXPECT_EQ(0, repository2.number_passwords);
	EXPECT_EQ(0, repository2.repository.load(&repository2.repository));
	EXPECT_EQ(2, repository2.number_passwords);

	repository2.destroy(&repository2);
	TEAR_DOWN;
}

TEST(kiPasswordMsgPackPersister, deserializationIsDeserializingGoodValueFor2Passwords) {
	SETUP;
	kiPasswordMsgPackPersister repository2;
	kiPasswordFactory factory2;
	kiki_pwd_mng_kiPasswordMsgPackPersister_init(&repository2, "shadow", &factory2);
	kiki_pwd_mng_kiPasswordFactory_init(&factory2, &repository2.repository, &encryptor, &decryptor);

	EXPECT_EQ(0, repository2.repository.load(&repository2.repository));

	EXPECT_EQ(0, memcmp(SAVED_UUID_1, repository2.password_array[0]->uuid, 16));
	EXPECT_EQ(0, memcmp(SAVED_IV_1, repository2.password_array[0]->iv, 16));
	EXPECT_EQ(0, strcmp(SAVED_NAME_1, repository2.password_array[0]->name));
	EXPECT_EQ(0, strcmp(SAVED_DESCRIPTION_1, repository2.password_array[0]->description));
	char passvalue[KIKI_PWD_MAX_VALUE_LEN];
	memset(passvalue, 0, KIKI_PWD_MAX_VALUE_LEN);
	memcpy(passvalue, SAVED_VALUE_1, 32);
	EXPECT_EQ(0, memcmp(passvalue, repository2.password_array[0]->value, KIKI_PWD_MAX_VALUE_LEN));

	EXPECT_EQ(0, memcmp(SAVED_UUID_2, repository2.password_array[1]->uuid, 16));
	EXPECT_EQ(0, memcmp(SAVED_IV_2, repository2.password_array[1]->iv, 16));
	EXPECT_EQ(0, strcmp(SAVED_NAME_2, repository2.password_array[1]->name));
	EXPECT_EQ(0, strcmp(SAVED_DESCRIPTION_2, repository2.password_array[1]->description));
	char passvalue2[KIKI_PWD_MAX_VALUE_LEN];
	memset(passvalue2, 0, KIKI_PWD_MAX_VALUE_LEN);
	memcpy(passvalue2, SAVED_VALUE_2, 32);
	EXPECT_EQ(0, memcmp(passvalue2, repository2.password_array[1]->value, KIKI_PWD_MAX_VALUE_LEN));

	repository2.destroy(&repository2);
	TEAR_DOWN;
}

TEST(kiPasswordMsgPackPersister, canRemove1PasswordWhen1Stored) {
	SETUP;
	kiPassword* password1 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password1);

	EXPECT_EQ(1, repository.number_passwords);
	EXPECT_EQ(0, repository.repository.remove(&repository.repository, password1->uuid));
	EXPECT_EQ(0, repository.number_passwords);
	TEAR_DOWN;
}

TEST(kiPasswordMsgPackPersister, canRemoveFirstPasswordWhen2Stored) {
	SETUP;
	kiPassword* password1 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password1);
	kiPassword* password2 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password2);

	EXPECT_EQ(2, repository.number_passwords);
	EXPECT_EQ(password1, repository.password_array[0]);
	EXPECT_EQ(0, repository.repository.remove(&repository.repository, password1->uuid));
	EXPECT_EQ(1, repository.number_passwords);
	EXPECT_EQ(password2, repository.password_array[0]);
	TEAR_DOWN;
}

TEST(kiPasswordMsgPackPersister, canRemoveSecondPasswordWhen2Stored) {
	SETUP;
	kiPassword* password1 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password1);
	kiPassword* password2 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password2);

	EXPECT_EQ(2, repository.number_passwords);
	EXPECT_EQ(password1, repository.password_array[0]);
	EXPECT_EQ(0, repository.repository.remove(&repository.repository, password2->uuid));
	EXPECT_EQ(1, repository.number_passwords);
	EXPECT_EQ(password1, repository.password_array[0]);
	TEAR_DOWN;
}

TEST(kiPasswordMsgPackPersister, canRemoveMiddlePasswordWhen3Stored) {
	SETUP;
	kiPassword* password1 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password1);
	kiPassword* password2 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password2);
	kiPassword* password3 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password3);

	EXPECT_EQ(3, repository.number_passwords);
	EXPECT_EQ(password2, repository.password_array[1]);
	EXPECT_EQ(0, repository.repository.remove(&repository.repository, password2->uuid));
	EXPECT_EQ(2, repository.number_passwords);
	EXPECT_EQ(password3, repository.password_array[1]);
	TEAR_DOWN;
}

TEST(kiPasswordMsgPackPersister, cannotRemovePasswordWhenInvalidUUID) {
	SETUP;
	kiPassword* password1 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password1);
	kiPassword* password2 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password2);
	kiPassword* password3 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);

	EXPECT_EQ(2, repository.number_passwords);
	EXPECT_EQ(-1, repository.repository.remove(&repository.repository, password3->uuid));
	EXPECT_EQ(2, repository.number_passwords);
	EXPECT_EQ(password2, repository.password_array[1]);
	kiki_pwd_mng_kiPassword_free(password3);
	TEAR_DOWN;
}

TEST(kiPasswordMsgPackPersister, canSortByNamesPasswordsWhen3Stored) {
	SETUP;
	kiPassword* password1 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password1);
	strcat(password1->name, "passC");
	kiPassword* password2 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password2);
	strcat(password2->name, "passA");
	kiPassword* password3 = kiki_pwd_mng_kiPassword_init(NULL, &encryptor, &decryptor, &repository.repository);
	repository.repository.add(&repository.repository, password3);
	strcat(password3->name, "passB");

	repository.repository.sort(&repository.repository);

	EXPECT_EQ(password2, repository.password_array[0]);
	EXPECT_EQ(password3, repository.password_array[1]);
	EXPECT_EQ(password1, repository.password_array[2]);
	TEAR_DOWN;
}