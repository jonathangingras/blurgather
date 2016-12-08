#ifndef BLURGATHER_SECRET_KEY_H
#define BLURGATHER_SECRET_KEY_H

#include <stdlib.h>
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

bg_secret_key_t *bg_secret_key_new(bg_context *ctx, const void *value, size_t length);
void bg_secret_key_free(bg_secret_key_t *self);

int bg_secret_key_update(bg_secret_key_t *self, const void *value, size_t length);
const void *bg_secret_key_data(const bg_secret_key_t *self);
size_t bg_secret_key_length(const bg_secret_key_t *self);

#ifdef __cplusplus
}
#endif

#endif
