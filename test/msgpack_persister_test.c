#include <sweetgreen/sweetgreen.h>
#include "mocks.h"
#include <blurgather/msgpack_persister.h>


#define TEST_FILE_PATH "/tmp/bg.shadow.bin.test"
bg_persister_t *persister = NULL;
bg_msgpack_persister *_persister = NULL;
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

sweetgreen_setup(persister) {
  turnoff_debug();

  reset_context();

  reset_allocator();
  bgctx_register_allocator(ctx, &mock_allocator);

  reset_mock_cryptor();
  bgctx_register_cryptor(ctx, &mock_cryptor);

  reset_mock_repository();
  bgctx_register_repository(ctx, &mock_repository);

  bg_string *filename = bg_string_from_str(TEST_FILE_PATH);
  _persister = bg_msgpack_persister_new(ctx, filename);

  persister = bg_msgpack_persister_persister(_persister);
  bgctx_register_persister(ctx, persister);

  pwd1 = bg_password_new(ctx);
  bg_password_update_name(pwd1,  bg_string_from_str("somename1"));
  bg_password_update_value(pwd1,  bg_string_from_str("somevalue1"));
  bg_password_update_description(pwd1, bg_string_from_str("somedesc1"));

  pwd2 = bg_password_new(ctx);
  bg_password_update_name(pwd2,  bg_string_from_str("somename2"));
  bg_password_update_value(pwd2,  bg_string_from_str("somevalue2"));
  bg_password_update_description(pwd2, bg_string_from_str("somedesc2"));

  pwd3 = bg_password_new(ctx);
  bg_password_update_name(pwd3,  bg_string_from_str("somename3"));
  bg_password_update_value(pwd3,  bg_string_from_str("somevalue3"));
  bg_password_update_description(pwd3, bg_string_from_str("somedesc3"));

  times_add_called = 0;

  reset_debug();
}

sweetgreen_teardown(persister) {
  if(_persister) {
    bg_persister_destroy(persister);
    free(_persister);
  }
  if(!access(TEST_FILE_PATH, F_OK)) {
    remove(TEST_FILE_PATH);
  }
  bg_password_free(pwd1);
  bg_password_free(pwd2);
  bg_password_free(pwd3);
}


sweetgreen_test_define(persister, persist_calls_repo_foreach) {
  bg_persister_persist(persister);

  sweetgreen_expect_true(mock_repository_foreach_called);
}

sweetgreen_test_define(persister, can_serialize_one_password) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_1;
  mock_repository_count_return_value = 1;

  sweetgreen_expect_equal(0, bg_persister_persist(persister));
  sweetgreen_expect_true(!access(TEST_FILE_PATH, F_OK));
}

sweetgreen_test_define(persister, can_deserialize_one_password) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_1;
  mock_repository_count_return_value = 1;
  bg_persister_persist(persister);

  sweetgreen_expect_equal(0, bg_persister_load(persister));
}

sweetgreen_test_define(persister, can_serialize_multiple_passwords) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_3;
  mock_repository_count_return_value = 3;

  sweetgreen_expect_equal(0, bg_persister_persist(persister));
  sweetgreen_expect_true(!access(TEST_FILE_PATH, F_OK));
}

sweetgreen_test_define(persister, can_deserialize_multiple_passwords) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_3;
  mock_repository_count_return_value = 3;
  bg_persister_persist(persister);

  sweetgreen_expect_equal(0, bg_persister_load(persister));
}

sweetgreen_test_define(persister, deserializing_one_password_calls_repository_add_once) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_1;
  mock_repository_count_return_value = 1;
  bg_persister_persist(persister);

  bg_persister_load(persister);

  sweetgreen_expect_equal(1, mock_repository_add_called);
}

sweetgreen_test_define(persister, deserializing_three_passwords_calls_repository_add_three_times) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_3;
  mock_repository_count_return_value = 3;
  bg_persister_persist(persister);

  bg_persister_load(persister);

  sweetgreen_expect_equal(3, mock_repository_add_called);
}

sweetgreen_test_define(persister, deserializing_one_password_calls_repository_add_with_good_password) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_1;
  mock_repository_count_return_value = 1;
  bg_persister_persist(persister);
  *((void**)&(mock_repository_vtable.add)) = &test_repo_add;

  bg_persister_load(persister);

  sweetgreen_expect_equal_string("somename1", bg_string_data(bg_password_name(pwds[0])));
  sweetgreen_expect_equal_string("somedesc1", bg_string_data(bg_password_description(pwds[0])));
  sweetgreen_expect_equal_string("somevalue1", bg_string_data(bg_password_value(pwds[0])));
  sweetgreen_expect_equal_string(mock_iv_data, bg_iv_data(bg_password_iv(pwds[0])));
}

sweetgreen_test_define(persister, deserializing_three_passwords_calls_repository_add_with_good_passwords) {
  *((void**)&(mock_repository_vtable.foreach)) = &test_repo_foreach_3;
  mock_repository_count_return_value = 3;
  bg_persister_persist(persister);
  *((void**)&(mock_repository_vtable.add)) = &test_repo_add;


  bg_persister_load(persister);


  sweetgreen_expect_equal_string("somename1", bg_string_data(bg_password_name(pwds[0])));
  sweetgreen_expect_equal_string("somedesc1", bg_string_data(bg_password_description(pwds[0])));
  sweetgreen_expect_equal_string("somevalue1", bg_string_data(bg_password_value(pwds[0])));
  sweetgreen_expect_equal_string(mock_iv_data, bg_iv_data(bg_password_iv(pwds[0])));

  sweetgreen_expect_equal_string("somename2", bg_string_data(bg_password_name(pwds[1])));
  sweetgreen_expect_equal_string("somedesc2", bg_string_data(bg_password_description(pwds[1])));
  sweetgreen_expect_equal_string("somevalue2", bg_string_data(bg_password_value(pwds[1])));
  sweetgreen_expect_equal_string(mock_iv_data, bg_iv_data(bg_password_iv(pwds[1])));

  sweetgreen_expect_equal_string("somename3", bg_string_data(bg_password_name(pwds[2])));
  sweetgreen_expect_equal_string("somedesc3", bg_string_data(bg_password_description(pwds[2])));
  sweetgreen_expect_equal_string("somevalue3", bg_string_data(bg_password_value(pwds[2])));
  sweetgreen_expect_equal_string(mock_iv_data, bg_iv_data(bg_password_iv(pwds[2])));
}
