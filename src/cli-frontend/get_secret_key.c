#include <blurgather/context.h>
#include <blurgather/secret_key.h>
#include "blur.h"


bg_secret_key_t *blur_get_secret_key(bg_context *ctx) {
  bg_string *key_value = blur_getfield("master password", 1);
  bg_secret_key_t *key = bg_secret_key_new(bg_string_data(key_value), bg_string_length(key_value));
  bg_string_clean_free(key_value);

  return key;
}
