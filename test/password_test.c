#include <sweetgreen/sweetgreen.h>

#include "blurgather/password.h"


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

sweetgreen_test_define(password, decryptIsCalledWhenCalledFromPassword) {
    bg_password_crypt(password);

    bg_password_decrypt(password);

    sweetgreen_expect_true(decryptor.decryptCalled);
}

sweetgreen_test_define(password, cannotDecryptWhenNULLDecryptor) {
    bg_password_destroy(password);
    bg_password_init(password, &dummy_iv_init, &encryptor.encryptor, NULL, &repository.repository);

    sweetgreen_expect_equal(-1, bg_password_decrypt(password));
}

sweetgreen_test_define(password, setIVIsCalledFromEncryptorWhenCallingCrypt) {
    sweetgreen_expect_false(encryptor.set_ivCalled);

    bg_password_crypt(password);

    sweetgreen_expect_true(encryptor.set_ivCalled);
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
