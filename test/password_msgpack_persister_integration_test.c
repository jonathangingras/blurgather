#include <sweetgreen/sweetgreen.h>

#include "password_msgpack_persister.h"

#include "dummy_iv.h"

bg_password_msgpack_persister repository;
bg_encryptor encryptor;
bg_decryptor decryptor;
bg_password_factory factory;

sweetgreen_setup(persister_integration_test) {
  bg_password_factory_init(&factory, &dummy_iv_init, NULL, &encryptor, &decryptor);
  bg_password_msgpack_persister_init(&repository, "shadow", &factory);
}

sweetgreen_teardown(persister_integration_test) {
  repository.destroy(&repository);
}

#define SAVED_NAME_1 "myPassName"
#define SAVED_DESCRIPTION_1 "some password description"
static char SAVED_IV_1[DUMMY_IV_LENGTH], SAVED_VALUE_1[32 + 1] = "12345678234567899876543212345678";

#define SAVED_NAME_2 "myPass2Name"
#define SAVED_DESCRIPTION_2 "some password2 description2"
static char SAVED_IV_2[DUMMY_IV_LENGTH], SAVED_VALUE_2[32 + 1] = "12345678234567899876543212345677";


sweetgreen_test_define(persister_integration_test, canSerializeContainedPasswords) {
  bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);

  memcpy(SAVED_IV_1, bg_password_iv_value(password), DUMMY_IV_LENGTH);
  bg_password_update_name(password, SAVED_NAME_1);
  bg_password_update_description(password, SAVED_DESCRIPTION_1);
  bg_password_update_value(password, SAVED_VALUE_1);
  
  repository.repository.add(&repository.repository, password);

  remove("shadow");
  sweetgreen_expect_true(access("shadow", F_OK) == -1 );
  sweetgreen_expect_not_null(repository.repository.persist);
        
  repository.repository.persist(&repository.repository);
  sweetgreen_expect_true(access("shadow", F_OK) == 0 );
}

sweetgreen_test_define(persister_integration_test, canDeserializeContainedPasswords) {
  bg_password_msgpack_persister repository2;
  bg_password_factory factory2;
  bg_password_msgpack_persister_init(&repository2, "shadow", &factory2);
  bg_password_factory_init(&factory2, &dummy_iv_init, &repository2.repository, &encryptor, &decryptor);

  sweetgreen_expect_equal(0, repository2.number_passwords);
  sweetgreen_expect_equal(0, repository2.repository.load(&repository2.repository));
  sweetgreen_expect_equal(1, repository2.number_passwords);

  repository2.destroy(&repository2);
}

sweetgreen_test_define(persister_integration_test, deserializationIsDeserializingGoodValue) {
  bg_password_msgpack_persister repository2;
  bg_password_factory factory2;
  bg_password_msgpack_persister_init(&repository2, "shadow", &factory2);
  bg_password_factory_init(&factory2, &dummy_iv_init, &repository2.repository, &encryptor, &decryptor);

  sweetgreen_expect_equal(0, repository2.repository.load(&repository2.repository));

  sweetgreen_expect_equal_memory(SAVED_IV_1, bg_password_iv_value(repository2.password_array[0]), DUMMY_IV_LENGTH);
  sweetgreen_expect_equal_string(SAVED_NAME_1, bg_password_name(repository2.password_array[0]));
  sweetgreen_expect_equal_string(SAVED_DESCRIPTION_1, bg_password_description(repository2.password_array[0]));
  char passvalue[BLURGATHER_PWD_MAX_VALUE_LEN];
  memset(passvalue, 0, BLURGATHER_PWD_MAX_VALUE_LEN);
  memcpy(passvalue, SAVED_VALUE_1, 32);
  sweetgreen_expect_equal_memory(passvalue, bg_password_value(repository2.password_array[0]), BLURGATHER_PWD_MAX_VALUE_LEN);

  repository2.destroy(&repository2);
}

sweetgreen_test_define(persister_integration_test, canSerialize2ContainedPasswords) {
  bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	
  memcpy(SAVED_IV_1, bg_password_iv_value(password), DUMMY_IV_LENGTH);
  bg_password_update_name(password, SAVED_NAME_1);
  bg_password_update_description(password, SAVED_DESCRIPTION_1);
  bg_password_update_value(password, SAVED_VALUE_1);
  repository.repository.add(&repository.repository, password);

  bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
	
  memcpy(SAVED_IV_2, bg_password_iv_value(password2), DUMMY_IV_LENGTH);
  bg_password_update_name(password2, SAVED_NAME_2);
  bg_password_update_description(password2, SAVED_DESCRIPTION_2);
  bg_password_update_value(password2, SAVED_VALUE_2);
  repository.repository.add(&repository.repository, password2);

  remove("shadow");
  sweetgreen_expect_equal(-1, access("shadow", F_OK));
  repository.repository.persist(&repository.repository);
  sweetgreen_expect_zero(access("shadow", F_OK));
}

sweetgreen_test_define(persister_integration_test, canDeserializeContained2Passwords) {
  bg_password_msgpack_persister repository2;
  bg_password_factory factory2;
  bg_password_msgpack_persister_init(&repository2, "shadow", &factory2);
  bg_password_factory_init(&factory2, &dummy_iv_init, &repository2.repository, &encryptor, &decryptor);

  sweetgreen_expect_zero(repository2.number_passwords);
  sweetgreen_expect_zero(repository2.repository.load(&repository2.repository));
  sweetgreen_expect_equal(2, repository2.number_passwords);

  repository2.destroy(&repository2);
}

sweetgreen_test_define(persister_integration_test, deserializationIsDeserializingGoodValueFor2Passwords) {
  bg_password_msgpack_persister repository2;
  bg_password_factory factory2;
  bg_password_msgpack_persister_init(&repository2, "shadow", &factory2);
  bg_password_factory_init(&factory2, &dummy_iv_init, &repository2.repository, &encryptor, &decryptor);

  sweetgreen_expect_zero(repository2.repository.load(&repository2.repository));

  sweetgreen_expect_equal_memory(SAVED_IV_1, bg_password_iv_value(repository2.password_array[0]), DUMMY_IV_LENGTH);
  sweetgreen_expect_equal_string(SAVED_NAME_1, bg_password_name(repository2.password_array[0]));
  sweetgreen_expect_equal_string(SAVED_DESCRIPTION_1, bg_password_description(repository2.password_array[0]));
  char passvalue[BLURGATHER_PWD_MAX_VALUE_LEN];
  memset(passvalue, 0, BLURGATHER_PWD_MAX_VALUE_LEN);
  memcpy(passvalue, SAVED_VALUE_1, 32);
  sweetgreen_expect_equal_memory(passvalue, bg_password_value(repository2.password_array[0]), BLURGATHER_PWD_MAX_VALUE_LEN);

  sweetgreen_expect_equal_memory(SAVED_IV_2, bg_password_iv_value(repository2.password_array[1]), DUMMY_IV_LENGTH);
  sweetgreen_expect_equal_string(SAVED_NAME_2, bg_password_name(repository2.password_array[1]));
  sweetgreen_expect_equal_string(SAVED_DESCRIPTION_2, bg_password_description(repository2.password_array[1]));
  char passvalue2[BLURGATHER_PWD_MAX_VALUE_LEN];
  memset(passvalue2, 0, BLURGATHER_PWD_MAX_VALUE_LEN);
  memcpy(passvalue2, SAVED_VALUE_2, 32);
  sweetgreen_expect_equal_memory(passvalue2, bg_password_value(repository2.password_array[1]), BLURGATHER_PWD_MAX_VALUE_LEN);

  repository2.destroy(&repository2);
}
