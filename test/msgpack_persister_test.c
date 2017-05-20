#include <sweetgreen/sweetgreen.h>
#include "mocks.h"
#include <blurgather/msgpack_persister.h>

#define TEST_FILE_PATH "/tmp/bg.shadow.bin.test"

bg_persister_t *persister = NULL;

sweetgreen_setup(msgpack_persister) {
  turnoff_debug();

  reset_context();

  reset_allocator();
  bgctx_register_allocator(ctx, &mock_allocator);

  reset_mock_cryptor();
  bgctx_register_cryptor(ctx, &mock_cryptor);

  reset_mock_repository();
  bgctx_register_repository(ctx, &mock_repository);

  bg_string *filename = bg_string_from_str(TEST_FILE_PATH);
  bg_msgpack_persister *_persister = bg_msgpack_persister_new(ctx, filename);
  bg_string_free(filename);
  persister = bg_msgpack_persister_persister(_persister);
  bgctx_register_persister(ctx, persister);

  reset_debug();
}

sweetgreen_teardown(msgpack_persister) {
  if(persister) { bg_persister_free(persister); }
  if(!access(TEST_FILE_PATH, F_OK)) { remove(TEST_FILE_PATH); }
}
