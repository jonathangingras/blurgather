#include <string.h>
#include <stdlib.h>
#include "blurgather/context.h"
#include "blurgather/mcrypt_cryptor.h"
#include "blurgather/password.h"
#include "blurgather/repository.h"

#define BGCTX_SEALED 0x1

struct bg_context {
  bg_repository_t *repository;
  bg_cryptor_t *cryptor;
  bg_allocator_t *allocator;
  bg_persister_t *persister;
  bg_secret_key_t *secret_key;
  int flags;
};

int bgctx_init(bg_context **ctx) {
  *ctx = malloc(sizeof(bg_context));
  memset(*ctx, 0, sizeof(bg_context));
  return 0;
}

int bgctx_seal(bg_context *ctx) {
  ctx->flags |= ctx->flags & BGCTX_SEALED;
  return 0;
}

int bgctx_sealed(bg_context *ctx) {
  return ctx->flags & BGCTX_SEALED;
}

static int check_ctx(bg_context *ctx) {
  if(!ctx) { return -1; }
  if(ctx->flags & BGCTX_SEALED) { return -2; }

  return 0;
}


int bgctx_register_allocator(bg_context *ctx, bg_allocator_t *allocator) {
  int error_code = 0;
  if((error_code = check_ctx(ctx))) { return error_code; }

  ctx->allocator = allocator;
  return 0;
}

int bgctx_register_repository(bg_context *ctx, bg_repository_t *repository) {
  int error_code = 0;
  if((error_code = check_ctx(ctx))) { return error_code; }

  ctx->repository = repository;
  return 0;
}

int bgctx_register_persister(bg_context *ctx, bg_persister_t *persister) {
  int error_code = 0;
  if((error_code = check_ctx(ctx))) { return error_code; }

  ctx->persister = persister;
  return 0;
}

int bgctx_register_cryptor(bg_context *ctx, bg_cryptor_t *cryptor) {
  int error_code = 0;
  if((error_code = check_ctx(ctx))) { return error_code; }

  ctx->cryptor = cryptor;
  return 0;
}

void *bgctx_allocate(bg_context *ctx, size_t size) {
  return ctx->allocator->allocate(size);
}

void bgctx_deallocate(bg_context *ctx, void* object) {
  return ctx->allocator->deallocate(object);
}

void *bgctx_reallocate(bg_context *ctx, void *object, size_t size) {
  return ctx->allocator->reallocate(object, size);
}

int bgctx_finalize(bg_context *ctx) {
  /*TODO: actually implement*/
  return 0;
}

int bgctx_unlock(bg_context *ctx, bg_secret_key_t *secret_key) {
  ctx->secret_key = secret_key;
  return 0;
}

int bgctx_lock(bg_context *ctx) {
  ctx->secret_key = NULL;
  return 0;
}

bg_secret_key_t *bgctx_access_key(bg_context *ctx) {
  return ctx->secret_key;
}

bg_repository_t *bgctx_repository(bg_context *ctx) {
  return ctx->repository;
}

bg_cryptor_t *bgctx_cryptor(bg_context *ctx) {
  return ctx->cryptor;
}

int bgctx_new_password(bg_context *ctx, bg_password **password) {
  *password = bg_password_new(ctx);
  return *password == NULL;
}

int bgctx_find_password(bg_context *ctx, const bg_string *name, bg_password **password) {
  return bg_repository_get(ctx->repository, name, password);
}

int bgctx_each_password(bg_context *ctx, int (* callback)(bg_password *password, void *), void *out) {
  return bg_repository_foreach(ctx->repository, callback, out);
}
