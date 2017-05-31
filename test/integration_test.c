#include <sweetgreen/sweetgreen.h>
#include <blurgather/string.h>
#include <blurgather/context.h>
#include <blurgather/mcrypt_cryptor.h>
#include <blurgather/array_repository.h>
#include <blurgather/msgpack_persister.h>


#define TEST_FILE_PATH "/tmp/bg.shadow.bin.integration_test"

bg_context *ctx;
bg_allocator_t bg_default_allocator = {
  .allocate = malloc,
  .deallocate = free,
  .reallocate = realloc,
};
bg_msgpack_persister *persister;
bg_password_array_repository *repository;

sweetgreen_setup(default_blur_setup) {
  bgctx_init(&ctx);
  bgctx_register_allocator(ctx, &bg_default_allocator);

  bgctx_register_cryptor(ctx, bg_mcrypt_cryptor());

  persister = bg_msgpack_persister_new(ctx, bg_string_from_str(TEST_FILE_PATH));
  bgctx_register_persister(ctx, bg_msgpack_persister_persister(persister));

  repository = bg_password_array_repository_init(NULL, ctx);
  bgctx_register_repository(ctx, bg_password_array_repository_repository(repository));

  bgctx_config(ctx, BGCTX_ACQUIRE_PERSISTER | BGCTX_ACQUIRE_REPOSITORY);
  bgctx_seal(ctx);
}

sweetgreen_teardown(default_blur_setup) {
  bgctx_finalize(ctx);

  remove(TEST_FILE_PATH);
}


sweetgreen_test_define(default_blur_setup, can_instantiate_a_password_and_save_it) {
  bg_password *pwd = bg_password_new(ctx);
  bg_password_update_name(pwd, bg_string_from_str("somepass"));
  bg_password_update_value(pwd, bg_string_from_str("somevalue"));
  bg_password_update_description(pwd, bg_string_from_str("somedesc"));

  bgctx_unlock(ctx, bg_secret_key_new("secret", 6));
  bg_password_crypt(pwd);
  bgctx_lock(ctx);

  bg_password_save(pwd);
  bg_persister_persist(bg_msgpack_persister_persister(persister));
}
