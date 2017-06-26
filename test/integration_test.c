#include <sweetgreen/sweetgreen.h>
#include <blurgather/string.h>
#include <blurgather/context.h>
#include <blurgather/mcrypt_cryptor.h>
#include <blurgather/array_repository.h>
#include <blurgather/msgpack_persister.h>


#define TEST_FILE_PATH "/tmp/bg.shadow.bin.integration_test"

bg_context *ctx;
bg_msgpack_persister *persister;

void setup_context(void) {
  bgctx_init(&ctx);

  bgctx_register_cryptor(ctx, bg_mcrypt_cryptor());

  persister = bg_msgpack_persister_new(bg_string_from_str(TEST_FILE_PATH));
  bgctx_register_persister(ctx, bg_msgpack_persister_persister(persister));

  bgctx_register_repository(ctx, bg_password_array_repository_new());

  bgctx_config(ctx, BGCTX_ACQUIRE_PERSISTER | BGCTX_ACQUIRE_REPOSITORY);
  bgctx_seal(ctx);
}

sweetgreen_setup(default_blur_setup) {
  setup_context();
}

sweetgreen_teardown(default_blur_setup) {
  bgctx_finalize(ctx);
  remove(TEST_FILE_PATH);
}

#define NB_PASS 500

int create_password_db(void) {
  int i;
  for(i = 0; i < NB_PASS; ++i) {
    bg_password *pwd = bg_password_new();
    bg_password_update_name(pwd, bg_string_plus(bg_string_from_str("somepass"), bg_string_from_decimal(i)));
    bg_password_update_value(pwd, bg_string_plus(bg_string_from_str("somevalue"), bg_string_from_decimal(i)));
    bg_password_update_description(pwd, bg_string_plus(bg_string_from_str("somedesc"), bg_string_from_decimal(i)));

    bgctx_unlock(ctx, bg_secret_key_new("secret", 6));
    bg_password_crypt(pwd, bgctx_cryptor(ctx), bgctx_access_key(ctx));
    bgctx_lock(ctx);

    bg_repository_add(bgctx_repository(ctx), pwd);
  }
  return bg_persister_persist(bg_msgpack_persister_persister(persister), bgctx_repository(ctx));
}


sweetgreen_test_define(default_blur_setup, can_instantiate_passwords_and_save_them) {
  sweetgreen_expect_zero(create_password_db());
}


bg_password *pwds[NB_PASS];
int nbpass;

int store_pass(bg_password *pwd, void *output) {
  pwds[nbpass] = pwd;
  ++nbpass;
  return 0;
}

sweetgreen_test_define(default_blur_setup, can_read_saved_passwords) {
  create_password_db();
  bgctx_finalize(ctx);
  setup_context();
  bg_persister_load(bg_msgpack_persister_persister(persister), bgctx_repository(ctx));

  nbpass = 0;
  bg_repository_foreach(bgctx_repository(ctx), &store_pass, NULL);

  int i;
  for(i = 0; i < 500; ++i) {
    bg_string *name = bg_string_plus(bg_string_from_str("somepass"), bg_string_from_decimal(i));
    bg_string *value = bg_string_plus(bg_string_from_str("somevalue"), bg_string_from_decimal(i));
    bg_string *desc = bg_string_plus(bg_string_from_str("somedesc"), bg_string_from_decimal(i));

    bgctx_unlock(ctx, bg_secret_key_new("secret", 6));

    bg_password_decrypt(pwds[i], bgctx_cryptor(ctx), bgctx_access_key(ctx));
    sweetgreen_expect_equal_string(bg_string_data(name), bg_string_data(bg_password_name(pwds[i])));
    sweetgreen_expect_equal_string(bg_string_data(value), bg_string_data(bg_password_value(pwds[i])));
    sweetgreen_expect_equal_string(bg_string_data(desc), bg_string_data(bg_password_description(pwds[i])));
    bg_password_crypt(pwds[i], bgctx_cryptor(ctx), bgctx_access_key(ctx));

    bgctx_lock(ctx);

    bg_string_free(name);
    bg_string_free(value);
    bg_string_free(desc);
  }
}
