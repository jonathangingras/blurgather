#ifndef BLURGATHER_CONTEXT_H
#define BLURGATHER_CONTEXT_H

#include "types.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BGCTX_ACQUIRE_ALLOCATOR 0x2
#define BGCTX_ACQUIRE_REPOSITORY 0x4
#define BGCTX_ACQUIRE_PERSISTER 0x8

/* manual initialization */
int bgctx_init(bg_context **ctx);

int bgctx_register_repository(bg_context *ctx, bg_repository_t *repository);
int bgctx_register_persister(bg_context *ctx, bg_persister_t *persister);
int bgctx_register_cryptor(bg_context *ctx, bg_cryptor_t *cryptor);
int bgctx_config(bg_context *ctx, int flags);
int bgctx_seal(bg_context *ctx);
int bgctx_sealed(bg_context *ctx);

/* finalization */
int bgctx_finalize(bg_context *ctx);

/* runtime context accessors */
bg_secret_key_t *bgctx_access_key(bg_context *ctx);
bg_repository_t *bgctx_repository(bg_context *ctx);
bg_cryptor_t *bgctx_cryptor(bg_context *ctx);

/* runtime password library lock/unlock */
int bgctx_unlock(bg_context *ctx, bg_secret_key_t *secret_key);
int bgctx_lock(bg_context *ctx);
int bgctx_locked(bg_context *ctx);

/* runtime password library manipulation shortcuts */
int bgctx_find_password(bg_context *ctx, const bg_string *name, bg_password **password);
int bgctx_each_password(bg_context *ctx, int (* callback)(bg_password *password, void *), void *out);
int bgctx_load(bg_context *ctx);
int bgctx_persist(bg_context *ctx);
int bgctx_add_password(bg_context *ctx, bg_password *password);
int bgctx_remove_password(bg_context *ctx, bg_string *name);
int bgctx_encrypt_password(bg_context *ctx, bg_password *password);
int bgctx_decrypt_password(bg_context *ctx, bg_password *password);

/* data memorization association facility */
int bgctx_register_memory(bg_context *ctx, bg_string *key, void *mem);
void *bgctx_get_memory(bg_context *ctx, bg_string *key);

#ifdef __cplusplus
}
#endif

#endif /* BLURGATHER_CONTEXT_H */
