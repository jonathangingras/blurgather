#ifndef BLURGATHER_CONTEXT_H
#define BLURGATHER_CONTEXT_H

#include "types.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_allocator_t;
typedef struct bg_allocator_t bg_allocator_t;
struct bg_allocator_t {
  void *(* const allocate)(size_t size);
  void (* const deallocate)(void *mem);
  void *(* const reallocate)(void *mem, size_t size);
};

/* manual initialization */
int bgctx_init(bg_context **ctx);
int bgctx_register_allocator(bg_context *ctx, bg_allocator_t *allocator);
int bgctx_register_repository(bg_context *ctx, bg_repository_t *repository);
int bgctx_register_persister(bg_context *ctx, bg_persister_t *persister);
int bgctx_register_cryptor(bg_context *ctx, bg_cryptor_t *cryptor);
int bgctx_seal(bg_context *ctx);
int bgctx_sealed(bg_context *ctx);

/* memory manipulation */
void *bgctx_allocate(bg_context *ctx, size_t size);
void bgctx_deallocate(bg_context *ctx, void *object);
void *bgctx_reallocate(bg_context *ctx, void *object, size_t size);

/* finalization */
int bgctx_finalize(bg_context *ctx);

/* runtime password library lock/unlock */
int bgctx_unlock(bg_context *ctx, bg_secret_key_t *secret_key);
int bgctx_lock(bg_context *ctx);

/* runtime context accessors */
bg_secret_key_t *bgctx_access_key(bg_context *ctx);
bg_repository_t *bgctx_repository(bg_context *ctx);
bg_cryptor_t *bgctx_cryptor(bg_context *ctx);

/* runtime password library manipulation shortcuts */
int bgctx_new_password(bg_context *ctx, bg_password **password);
int bgctx_find_password(bg_context *ctx, const bg_string *name, bg_password **password);
int bgctx_each_password(bg_context *ctx, int (* callback)(bg_password *password, void *), void *out);

#ifdef __cplusplus
}
#endif

#endif /* BLURGATHER_CONTEXT_H */
