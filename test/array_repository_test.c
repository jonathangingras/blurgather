#include <sweetgreen/sweetgreen.h>

#include <blurgather/array_repository.h>
#include "mocks.h"


bg_repository_t *repo;

sweetgreen_setup(array_repository) {
  turnoff_debug();

  reset_context();

  reset_allocator();
  bgctx_register_allocator(ctx, &mock_allocator);

  reset_mock_cryptor();
  bgctx_register_cryptor(ctx, &mock_cryptor);

  bg_password_array_repository *repo_ = bg_password_array_repository_init(NULL, ctx);
  repo = bg_password_array_repository_repository(repo_);

  bgctx_register_repository(ctx, repo);

  reset_debug();
}

sweetgreen_test_define(array_repository, cannot_add_a_empty_name_password) {
  bg_password *pwd = bg_password_new(ctx);

  sweetgreen_expect_non_zero(bg_repository_add(repo, pwd));
  sweetgreen_expect_equal(0, bg_repository_count(repo));
}

sweetgreen_test_define(array_repository, can_add_a_password_when_empty) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);

  sweetgreen_expect_zero(bg_repository_add(repo, pwd));
  sweetgreen_expect_equal(1, bg_repository_count(repo));
}

sweetgreen_test_define(array_repository, can_get_an_added_password_when_1_stored) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);
  bg_repository_add(repo, pwd);

  bg_password *res;
  sweetgreen_expect_zero(bg_repository_get(repo, name, &res));
}

sweetgreen_test_define(array_repository, gotten_added_password_has_same_address_as_the_one_added) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);
  bg_repository_add(repo, pwd);
  bg_password *res;
  bg_repository_get(repo, name, &res);

  sweetgreen_expect_same_address(pwd, res);
}

sweetgreen_test_define(array_repository, can_add_a_password_when_1_stored) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);

  bg_repository_add(repo, pwd);

  bg_password *pwd2 = bg_password_new(ctx);
  bg_string *name2 = bg_string_from_str("somepassname2");
  bg_password_update_name(pwd2, name2);

  sweetgreen_expect_zero(bg_repository_add(repo, pwd2));
  sweetgreen_expect_equal(2, bg_repository_count(repo));
}

sweetgreen_test_define(array_repository, can_get_all_added_passwords_when_2_stored) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);
  bg_repository_add(repo, pwd);
  bg_password *pwd2 = bg_password_new(ctx);
  bg_string *name2 = bg_string_from_str("somepassname2");
  bg_password_update_name(pwd2, name2);
  bg_repository_add(repo, pwd2);

  bg_password *res;
  sweetgreen_expect_zero(bg_repository_get(repo, name, &res));
  sweetgreen_expect_zero(bg_repository_get(repo, name2, &res));
}

sweetgreen_test_define(array_repository, all_gotten_added_passwords_have_same_address_as_the_ones_added) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);
  bg_repository_add(repo, pwd);
  bg_password *pwd2 = bg_password_new(ctx);
  bg_string *name2 = bg_string_from_str("somepassname2");
  bg_password_update_name(pwd2, name2);
  bg_repository_add(repo, pwd2);

  bg_password *res, *res2;
  bg_repository_get(repo, name, &res);
  bg_repository_get(repo, name2, &res2);
  sweetgreen_expect_same_address(pwd, res);
  sweetgreen_expect_same_address(pwd2, res2);
}

sweetgreen_test_define(array_repository, cannot_add_password_of_same_name_twice) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);
  bg_password *pwd2 = bg_password_new(ctx);
  bg_password_update_name(pwd2, name);

  sweetgreen_expect_zero(bg_repository_add(repo, pwd));
  sweetgreen_expect_non_zero(bg_repository_add(repo, pwd2));
  sweetgreen_expect_equal(1, bg_repository_count(repo));
}

sweetgreen_test_define(array_repository, can_remove_password_when_1_stored) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);
  bg_repository_add(repo, pwd);

  sweetgreen_expect_zero(bg_repository_remove(repo, name));
}

sweetgreen_test_define(array_repository, can_remove_password_when_2_stored) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);
  bg_password *pwd2 = bg_password_new(ctx);
  bg_string *name2 = bg_string_from_str("somepassname2");
  bg_password_update_name(pwd2, name2);
  bg_repository_add(repo, pwd);
  bg_repository_add(repo, pwd2);

  sweetgreen_expect_zero(bg_repository_remove(repo, name2));
}

sweetgreen_test_define(array_repository, count_decreases_when_remove_password_given_a_1_stored_repository) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);
  bg_repository_add(repo, pwd);

  sweetgreen_expect_equal(1, bg_repository_count(repo));
  bg_repository_remove(repo, name);
  sweetgreen_expect_equal(0, bg_repository_count(repo));
}

sweetgreen_test_define(array_repository, count_decreases_when_remove_password_given_a_2_stored_repository) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);
  bg_password *pwd2 = bg_password_new(ctx);
  bg_string *name2 = bg_string_from_str("somepassname2");
  bg_password_update_name(pwd2, name2);
  bg_repository_add(repo, pwd);
  bg_repository_add(repo, pwd2);

  sweetgreen_expect_equal(2, bg_repository_count(repo));
  bg_repository_remove(repo, name2);
  sweetgreen_expect_equal(1, bg_repository_count(repo));
}

sweetgreen_test_define(array_repository, cannot_get_removed_when_remove_password_given_a_1_stored_repository) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);
  bg_repository_add(repo, pwd);

  bg_repository_remove(repo, name);

  bg_password *res;
  sweetgreen_expect_non_zero(bg_repository_get(repo, name, &res));
}

sweetgreen_test_define(array_repository, cannot_get_removed_when_remove_password_given_a_2_stored_repository) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);
  bg_password *pwd2 = bg_password_new(ctx);
  bg_string *name2 = bg_string_from_str("somepassname2");
  bg_password_update_name(pwd2, name2);
  bg_repository_add(repo, pwd);
  bg_repository_add(repo, pwd2);

  bg_repository_remove(repo, name2);

  bg_password *res;
  sweetgreen_expect_non_zero(bg_repository_get(repo, name2, &res));
}

sweetgreen_test_define(array_repository, can_get_non_removed_when_remove_password_given_a_2_stored_repository) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);
  bg_password *pwd2 = bg_password_new(ctx);
  bg_string *name2 = bg_string_from_str("somepassname2");
  bg_password_update_name(pwd2, name2);
  bg_repository_add(repo, pwd);
  bg_repository_add(repo, pwd2);

  bg_repository_remove(repo, name2);

  bg_password *res;
  sweetgreen_expect_zero(bg_repository_get(repo, name, &res));
  sweetgreen_expect_same_address(pwd, res);
}


int times_called = 0;
bg_password *pwds[3];
int iterator_callback(bg_password *pwd, void *output) {
  pwds[times_called] = pwd;
  ++times_called;
  return 0;
}

sweetgreen_test_define(array_repository, can_iterate_over_repository_given_3_stored_passwords) {
  bg_password *pwd = bg_password_new(ctx);
  bg_string *name = bg_string_from_str("somepassname");
  bg_password_update_name(pwd, name);
  bg_password *pwd2 = bg_password_new(ctx);
  bg_string *name2 = bg_string_from_str("somepassname2");
  bg_password_update_name(pwd2, name2);
  bg_password *pwd3 = bg_password_new(ctx);
  bg_string *name3 = bg_string_from_str("somepassname3");
  bg_password_update_name(pwd3, name3);
  bg_repository_add(repo, pwd);
  bg_repository_add(repo, pwd2);
  bg_repository_add(repo, pwd3);

  bg_repository_foreach(repo, &iterator_callback, NULL);

  sweetgreen_expect_equal_string("somepassname", bg_string_data(bg_password_name(pwds[0])));
  sweetgreen_expect_equal_string("somepassname2", bg_string_data(bg_password_name(pwds[1])));
  sweetgreen_expect_equal_string("somepassname3", bg_string_data(bg_password_name(pwds[2])));
}
