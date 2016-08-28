#ifndef BLURGATHER_SECRET_KEY_H
#define BLURGATHER_SECRET_KEY_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bg_secret_key;
typedef struct bg_secret_key bg_secret_key_t;

bg_secret_key_t* bg_secret_key_new(const void *value, size_t length);
void bg_secret_key_free(bg_secret_key_t *self);
void bg_secret_key_destroy(bg_secret_key_t *self);

int bg_secret_key_update(bg_secret_key_t *self, const void *value, size_t length);
const void *bg_secret_key_data(bg_secret_key_t *self);
size_t bg_secret_key_length(bg_secret_key_t *self);

#ifdef __cplusplus
}
#endif

#endif //BLURGATHER_SECRET_KEY_H
