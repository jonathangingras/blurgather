#ifndef _BLUR_H_
#define _BLUR_H_

#include <blurgather/context.h>

#ifdef __cplusplus
extern "C" {
#endif

int blur_create_context(bg_context **ctx,
                        bg_persister_t *persister,
                        bg_repository_t *repo,
                        bg_cryptor_t *cryptor);

bg_string *blur_getfield(const char *showing, int hide_input);

bg_secret_key_t *blur_get_secret_key(bg_context *ctx);

#ifdef __cplusplus
}
#endif

#endif
