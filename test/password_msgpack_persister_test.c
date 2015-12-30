#include <sweetgreen/sweetgreen.h>

#include "password_msgpack_persister.h"

#include "dummy_iv.h"

bg_password_msgpack_persister repository;
bg_encryptor encryptor;
bg_decryptor decryptor;
bg_password_factory factory;

sweetgreen_setup(persister) {
  bg_password_factory_init(&factory, &dummy_iv_init, NULL, &encryptor, &decryptor);
  bg_password_msgpack_persister_init(&repository, "shadow", &factory);
}

sweetgreen_teardown(persister) {
  repository.destroy(&repository);
}

sweetgreen_test_define(persister, canAddPassword) {
  bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);

  sweetgreen_expect_equal(0, repository.number_passwords);

  repository.repository.add(&repository.repository, password);

  sweetgreen_expect_equal(1, repository.number_passwords);

  printf("hello\n");
}

sweetgreen_test_define(persister, canAdd2Passwords) {
  bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
  password->name = "1";
  bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
  password2->name = "2";

  sweetgreen_expect_equal(0, repository.number_passwords);

  repository.repository.add(&repository.repository, password);

  sweetgreen_expect_equal(1, repository.number_passwords);

  repository.repository.add(&repository.repository, password2);

  sweetgreen_expect_equal(2, repository.number_passwords);
}

sweetgreen_test_define(persister, passwordHasGoodPointerWhenAdded) {
  bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);

  repository.repository.add(&repository.repository, password);

  sweetgreen_expect_same_address(password, repository.password_array[0]);
}

sweetgreen_test_define(persister, passwordNotAddedTwiceWhenSavedMoreThanOnce) {
  bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, &repository.repository);
  password->name = "1";

  repository.repository.add(&repository.repository, password);
  repository.repository.add(&repository.repository, password);
  repository.repository.add(&repository.repository, password);
  repository.repository.add(&repository.repository, password);

  sweetgreen_expect_equal(1, repository.number_passwords);
}

