#include <string.h>
#include <blurgather/iv.h>

struct bg_iv {
  size_t length;
  void *data;
};

bg_iv_t *bg_iv_new(bg_context *ctx, const void *data, size_t length) {
  bg_iv_t *iv = bgctx_allocate(ctx, sizeof(bg_iv_t));

  iv->data = bgctx_allocate(ctx, length);
  memcpy(iv->data, data, length);
  iv->length = length;

  return iv;
}

const void *bg_iv_data(const bg_iv_t *iv) {
  return iv->data;
}

size_t bg_iv_length(const bg_iv_t *iv) {
  return iv->length;
}

void bg_iv_free(bg_iv_t *iv) {
  memset((void*)iv->data, 0, iv->length);
  free((void*)iv->data);
  free(iv);
}
