#include <sweetgreen/sweetgreen.h>

#include "blurgather/password_msgpack_persister.h"

#include "dummy_iv.h"

bg_password_msgpack_persister persister;
bg_password_repository *repository;
bg_encryptor encryptor;
bg_decryptor decryptor;
bg_password_factory factory;

sweetgreen_setup(persister) {
  bg_password_factory_init(&factory, &dummy_iv_init, NULL, &encryptor, &decryptor);
  bg_password_msgpack_persister_init(&persister, "shadow", &factory);
  repository = &persister.repository;
}

sweetgreen_teardown(persister) {
  bg_pwd_repository_destroy(repository);
}

sweetgreen_test_define(persister, canAddPassword) {
  bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
  bg_password_update_name(password, "1");

  sweetgreen_expect_equal(0, persister.number_passwords);

  bg_pwd_repository_add(repository, password);

  sweetgreen_expect_equal(1, persister.number_passwords);
}

sweetgreen_test_define(persister, canAdd2Passwords) {
  bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
  bg_password_update_name(password, "1");
  bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
  bg_password_update_name(password2, "2");

  sweetgreen_expect_equal(0, persister.number_passwords);

  bg_pwd_repository_add(repository, password);

  sweetgreen_expect_equal(1, persister.number_passwords);

  bg_pwd_repository_add(repository, password2);

  sweetgreen_expect_equal(2, persister.number_passwords);
}

sweetgreen_test_define(persister, passwordHasGoodPointerWhenAdded) {
  bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);

  bg_pwd_repository_add(repository, password);

  sweetgreen_expect_same_address(password, persister.password_array[0]);
}

sweetgreen_test_define(persister, passwordNotAddedTwiceWhenSavedMoreThanOnce) {
  bg_password* password = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
  bg_password_update_name(password, "1");

  bg_pwd_repository_add(repository, password);
  bg_pwd_repository_add(repository, password);
  bg_pwd_repository_add(repository, password);
  bg_pwd_repository_add(repository, password);

  sweetgreen_expect_equal(1, persister.number_passwords);
}

sweetgreen_test_define(persister, canRemove1PasswordWhen1Stored) {
	bg_password* password1 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
        bg_password_update_name(password1, "1");

	bg_pwd_repository_add(repository, password1);

	sweetgreen_expect_equal(1, persister.number_passwords);
	sweetgreen_expect_zero(bg_pwd_repository_remove(repository, bg_password_name(password1)));
	sweetgreen_expect_equal(0, persister.number_passwords);
}

sweetgreen_test_define(persister, canRemoveFirstPasswordWhen2Stored) {
	bg_password* password1 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
        bg_password_update_name(password1, "1");

	bg_pwd_repository_add(repository, password1);
	bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
        bg_password_update_name(password2, "2");
        
	bg_pwd_repository_add(repository, password2);

	sweetgreen_expect_equal(2, persister.number_passwords);
	sweetgreen_expect_same_address(password1, persister.password_array[0]);
	sweetgreen_expect_zero(bg_pwd_repository_remove(repository, bg_password_name(password1)));
	sweetgreen_expect_equal(1, persister.number_passwords);
	sweetgreen_expect_same_address(password2, persister.password_array[0]);
}

sweetgreen_test_define(persister, canRemoveSecondPasswordWhen2Stored) {
	bg_password* password1 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
        bg_password_update_name(password1, "1");
	bg_pwd_repository_add(repository, password1);
	bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
        bg_password_update_name(password2, "2");
	bg_pwd_repository_add(repository, password2);

        sweetgreen_expect_equal(2, persister.number_passwords);
	sweetgreen_expect_same_address(password1, persister.password_array[0]);
	sweetgreen_expect_zero(bg_pwd_repository_remove(repository, bg_password_name(password2)));
        sweetgreen_expect_equal(1, persister.number_passwords);
	sweetgreen_expect_same_address(password1, persister.password_array[0]);
}

sweetgreen_test_define(persister, canRemoveMiddlePasswordWhen3Stored) {
  bg_password* password1 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
  bg_password_update_name(password1, "1");
  bg_pwd_repository_add(repository, password1);
  bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
  bg_password_update_name(password2, "2");
  bg_pwd_repository_add(repository, password2);
  bg_password* password3 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
  bg_password_update_name(password3, "3");
  bg_pwd_repository_add(repository, password3);

  sweetgreen_expect_equal(3, persister.number_passwords);
  sweetgreen_expect_same_address(password2, persister.password_array[1]);
	
  sweetgreen_expect_zero(bg_pwd_repository_remove(repository, bg_password_name(password2)));

  sweetgreen_expect_equal(2, persister.number_passwords);
  sweetgreen_expect_same_address(password3, persister.password_array[1]);
}

sweetgreen_test_define(persister, cannotRemovePasswordWhenInvalidName) {
	bg_password* password1 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
        bg_password_update_name(password1, "1");
	bg_pwd_repository_add(repository, password1);
	bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
        bg_password_update_name(password2, "2");
	bg_pwd_repository_add(repository, password2);
	bg_password* password3 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
        bg_password_update_name(password1, "3");

	sweetgreen_expect_equal(2, persister.number_passwords);
	sweetgreen_expect_non_zero(bg_pwd_repository_remove(repository, bg_password_name(password3)));
	sweetgreen_expect_equal(2, persister.number_passwords);
	sweetgreen_expect_same_address(password2, persister.password_array[1]);
	
        bg_password_free(password3);
}

sweetgreen_test_define(persister, canSortByNamesPasswordsWhen3Stored) {
		bg_password* password1 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
	bg_pwd_repository_add(repository, password1);
	bg_password_update_name(password1, "passC");
	bg_password* password2 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
	bg_pwd_repository_add(repository, password2);
	bg_password_update_name(password2, "passA");
	bg_password* password3 = bg_password_init(NULL, &dummy_iv_init, &encryptor, &decryptor, repository);
	bg_pwd_repository_add(repository, password3);
	bg_password_update_name(password3, "passB");

	bg_pwd_repository_sort(repository);

	sweetgreen_expect_same_address(password2, persister.password_array[0]);
	sweetgreen_expect_same_address(password3, persister.password_array[1]);
	sweetgreen_expect_same_address(password1, persister.password_array[2]);
}
