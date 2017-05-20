#include <string.h>
#include <blurgather/iv.h>

struct bg_iv {
  bg_context *ctx;
  size_t length;
  void *data;
};

bg_iv_t *bg_iv_new(bg_context *ctx, const void *data, size_t length) {
  bg_iv_t *iv = bgctx_allocate(ctx, sizeof(bg_iv_t));

  iv->ctx = ctx;
  iv->data = bgctx_allocate(ctx, length);
  memcpy(iv->data, data, length);
  iv->length = length;

  return iv;
}

bg_iv_t *bg_iv_copy(bg_iv_t *iv) {
  return bg_iv_new(iv->ctx, iv->data, iv->length);
}

const void *bg_iv_data(const bg_iv_t *iv) {
  return iv->data;
}

size_t bg_iv_length(const bg_iv_t *iv) {
  return iv->length;
}

void bg_iv_free(bg_iv_t *iv) {
  memset((void*)iv->data, 0, iv->length);
  bgctx_deallocate(iv->ctx, (void*)iv->data);
  bgctx_deallocate(iv->ctx, iv);
}
