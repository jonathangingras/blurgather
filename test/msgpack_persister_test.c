#include <prufen/prufen.h>
#include "mocks.h"
#include <blurgather/msgpack_persister.h>


#define TEST_FILE_PATH "/tmp/bg.shadow.bin.test"
bg_persister_t *persister = NULL;
bg_password *pwd1;
bg_password *pwd2;
bg_password *pwd3;


int test_repo_foreach_1(bg_repository_t *self, int (* callback)(bg_password *, void *), void *output) {
  callback(pwd1, output);

  return 0;
}

int test_repo_foreach_3(bg_repository_t *self, int (* callback)(bg_password *, void *), void *output) {
  callback(pwd1, output);
  callback(pwd2, output);
  callback(pwd3, output);

  return 0;
}

int times_add_called;
bg_password* pwds[3];
int test_repo_add(bg_repository_t *repo, bg_password *password) {
  pwds[times_add_called] = password;

  ++times_add_called;
  return 0;
}

pruf_setup(persister) {
  turnoff_debug();

  if(!access(TEST_FILE_PATH, F_OK)) {
    remove(TEST_FILE_PATH);
  }

  bg_string *filename = bg_string_from_str(TEST_FILE_PATH);
  persister = bg_msgpack_persister_persister(bg_msgpack_persister_new(filename, &mock_cryptor));

  pwd1 = bg_password_new();
  bg_password_update_name(pwd1, bg_string_from_str("somename1"));
  bg_password_update_value(pwd1, bg_string_from_str("somevalue1"));
  bg_password_update_description(pwd1, bg_string_from_str("somedesc1"));

  pwd2 = bg_password_new();
  bg_password_update_name(pwd2, bg_string_from_str("somename2"));
  bg_password_update_value(pwd2, bg_string_from_str("somevalue2"));
  bg_password_update_description(pwd2, bg_string_from_str("somedesc2"));

  pwd3 = bg_password_new();
  bg_password_update_name(pwd3, bg_string_from_str("somename3"));
  bg_password_update_value(pwd3, bg_string_from_str("somevalue3"));
  bg_password_update_description(pwd3, bg_string_from_str("somedesc3"));

  times_add_called = 0;

  reset_debug();
}

pruf_teardown(persister) {
  if(persister) {
    bg_persister_destroy(persister);
    free((void*)persister->object);
  }
  if(!access(TEST_FILE_PATH, F_OK)) {
    remove(TEST_FILE_PATH);
  }
  bg_password_free(pwd1);
  bg_password_free(pwd2);
  bg_password_free(pwd3);
}


pruf_test_define(persister, persist_calls_repo_foreach) {
  bg_persister_persist(persister, &mock_repository);

  pruf_expect_true(mock_repository_foreach_called);
}

pruf_test_define(persister, can_serialize_one_password) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_1;
  mock_repository_count_return_value = 1;

  pruf_expect_equal(0, bg_persister_persist(persister, &mock_repository));
  pruf_expect_equal(0, access(TEST_FILE_PATH, F_OK));
}

pruf_test_define(persister, can_deserialize_one_password) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_1;
  mock_repository_count_return_value = 1;
  bg_persister_persist(persister, &mock_repository);

  pruf_expect_equal(0, bg_persister_load(persister, &mock_repository));
}

pruf_test_define(persister, can_serialize_multiple_passwords) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_3;
  mock_repository_count_return_value = 3;

  pruf_expect_equal(0, bg_persister_persist(persister, &mock_repository));
  pruf_expect_true(!access(TEST_FILE_PATH, F_OK));
}

pruf_test_define(persister, can_deserialize_multiple_passwords) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_3;
  mock_repository_count_return_value = 3;
  bg_persister_persist(persister, &mock_repository);

  pruf_expect_equal(0, bg_persister_load(persister, &mock_repository));
}

pruf_test_define(persister, deserializing_one_password_calls_repository_add_once) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_1;
  mock_repository_count_return_value = 1;
  bg_persister_persist(persister, &mock_repository);

  bg_persister_load(persister, &mock_repository);

  pruf_expect_equal(1, mock_repository_add_called);
}

pruf_test_define(persister, deserializing_three_passwords_calls_repository_add_three_times) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_3;
  mock_repository_count_return_value = 3;
  bg_persister_persist(persister, &mock_repository);

  bg_persister_load(persister, &mock_repository);

  pruf_expect_equal(3, mock_repository_add_called);
}

pruf_test_define(persister, deserializing_one_password_calls_repository_add_with_good_password) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_1;
  mock_repository_count_return_value = 1;
  bg_persister_persist(persister, &mock_repository);
  *((void**)&(mock_repository_vtable.add)) = &test_repo_add;

  bg_persister_load(persister, &mock_repository);

  pruf_expect_equal_string("somename1", bg_string_data(bg_password_name(pwds[0])));
  pruf_expect_equal_string("somedesc1", bg_string_data(bg_password_description(pwds[0])));
  pruf_expect_equal_string("somevalue1", bg_string_data(bg_password_value(pwds[0])));
}

pruf_test_define(persister, deserializing_three_passwords_calls_repository_add_with_good_passwords) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_3;
  mock_repository_count_return_value = 3;
  bg_persister_persist(persister, &mock_repository);
  *((void**)&(mock_repository_vtable.add)) = &test_repo_add;


  bg_persister_load(persister, &mock_repository);


  pruf_expect_equal_string("somename1", bg_string_data(bg_password_name(pwds[0])));
  pruf_expect_equal_string("somedesc1", bg_string_data(bg_password_description(pwds[0])));
  pruf_expect_equal_string("somevalue1", bg_string_data(bg_password_value(pwds[0])));

  pruf_expect_equal_string("somename2", bg_string_data(bg_password_name(pwds[1])));
  pruf_expect_equal_string("somedesc2", bg_string_data(bg_password_description(pwds[1])));
  pruf_expect_equal_string("somevalue2", bg_string_data(bg_password_value(pwds[1])));

  pruf_expect_equal_string("somename3", bg_string_data(bg_password_name(pwds[2])));
  pruf_expect_equal_string("somedesc3", bg_string_data(bg_password_description(pwds[2])));
  pruf_expect_equal_string("somevalue3", bg_string_data(bg_password_value(pwds[2])));
}
