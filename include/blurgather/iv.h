#ifndef BLURGATHER_IV_H
#define BLURGATHER_IV_H

#include <stdlib.h>
#include <blurgather/context.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bg_iv;
typedef struct bg_iv bg_iv_t;

bg_iv_t *bg_iv_new(bg_context *ctx, const void *data, size_t length);
const void *bg_iv_data(const bg_iv_t *iv);
size_t bg_iv_length(const bg_iv_t *iv);
void bg_iv_free(bg_iv_t *iv);

#ifdef __cplusplus
}
#endif

#endif /* BLURGATHER_IV_H */
