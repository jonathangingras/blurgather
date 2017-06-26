#include <string.h>
#include <stdlib.h>
#include "blurgather/context.h"
#include "blurgather/mcrypt_cryptor.h"
#include "blurgather/password.h"
#include "blurgather/repository.h"
#include "blurgather/persister.h"
#include "blurgather/map.h"


#define BGCTX_SEALED 0x1


struct bg_context {
  bg_repository_t *repository;
  bg_cryptor_t *cryptor;
  bg_persister_t *persister;
  bg_secret_key_t *secret_key;
  bg_map *map;
  int flags;
};

#define RETURN_IF_UNSEALED(ctx) if(!bgctx_sealed(ctx)) {return -1;}
#define RETURN_IF_LOCKED(ctx) if(!(ctx)->secret_key) {return -2;}

int bgctx_init(bg_context **ctx) {
  *ctx = malloc(sizeof(bg_context));
  memset(*ctx, 0, sizeof(bg_context));
  (*ctx)->map = bg_map_new();
  return 0;
}

int bgctx_seal(bg_context *ctx) {
  if(bgctx_sealed(ctx)) {
    return -1;
  }
  ctx->flags |= BGCTX_SEALED;
  return 0;
}

int bgctx_sealed(bg_context *ctx) {
  return ctx->flags & BGCTX_SEALED;
}

int bgctx_config(bg_context *ctx, int flags) {
  if(ctx->flags & BGCTX_SEALED) {
    return -1;
  }
  if(flags & BGCTX_SEALED) {
    return -2;
  }
  ctx->flags |= flags;
  return 0;
}

static int check_ctx(bg_context *ctx) {
  if(!ctx) { return -1; }
  if(ctx->flags & BGCTX_SEALED) { return -2; }

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

int bgctx_finalize(bg_context *ctx) {
  bgctx_lock(ctx);

  if(ctx->repository && (ctx->flags & BGCTX_ACQUIRE_REPOSITORY)) {
    bg_repository_destroy(ctx->repository);
    free((void*)ctx->repository->object);
    ctx->repository = NULL;
  }
  if(ctx->persister && (ctx->flags & BGCTX_ACQUIRE_PERSISTER)) {
    bg_persister_destroy(ctx->persister);
    free((void*)ctx->persister->object);
    ctx->repository = NULL;
  }
  bg_map_free(ctx->map);

  free(ctx);

  return 0;
}

int bgctx_unlock(bg_context *ctx, bg_secret_key_t *secret_key) {
  ctx->secret_key = secret_key;
  return 0;
}

int bgctx_lock(bg_context *ctx) {
  if(ctx->secret_key) {
    bg_secret_key_free(ctx->secret_key);
    ctx->secret_key = NULL;
  }
  return 0;
}

int bgctx_locked(bg_context *ctx) {
  return ctx->secret_key == NULL;
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

struct find_data {
  bg_context *ctx;
  const bg_string *name;
  bg_password *output;
};

static int password_find(bg_password *pwd, void *data) {
  int err = 0;
  bg_password *copy = bg_password_copy(pwd);

  if((err = bg_password_decrypt(copy, ((struct find_data*)data)->ctx->cryptor, ((struct find_data*)data)->ctx->secret_key))) {
    bg_password_free(copy);
    return err;
  }

  if(bg_string_compare(((struct find_data*)data)->name, bg_password_name(copy)) == 0) {
    ((struct find_data*)data)->output = copy;
    return 1;
  } else {
    bg_password_free(copy);
  }

  return 0;
}

int bgctx_find_password(bg_context *ctx, const bg_string *name, bg_password **password) {
  RETURN_IF_UNSEALED(ctx);
  RETURN_IF_LOCKED(ctx);

  int err = 0;
  struct find_data data = {
    .ctx = ctx,
    .name = name,
    .output = NULL
  };

  if((err = bg_repository_foreach(ctx->repository, &password_find, &data)) == 1) {
    *password = data.output;
    return 0;
  } else {
    return err;
  }
}

int bgctx_each_password(bg_context *ctx, int (* callback)(bg_password *password, void *), void *out) {
  RETURN_IF_UNSEALED(ctx);
  return bg_repository_foreach(ctx->repository, callback, out);
}

int bgctx_load(bg_context *ctx) {
  RETURN_IF_UNSEALED(ctx);
  return bg_persister_load(ctx->persister, ctx->repository);
}

int bgctx_persist(bg_context *ctx) {
  RETURN_IF_UNSEALED(ctx);
  return bg_persister_persist(ctx->persister, ctx->repository);
}

int bgctx_add_password(bg_context *ctx, bg_password *password) {
  RETURN_IF_UNSEALED(ctx);
  return bg_repository_add(ctx->repository, password);
}

int bgctx_encrypt_password(bg_context *ctx, bg_password *password) {
  RETURN_IF_UNSEALED(ctx);
  RETURN_IF_LOCKED(ctx);
  return bg_password_crypt(password, ctx->cryptor, ctx->secret_key);
}

int bgctx_decrypt_password(bg_context *ctx, bg_password *password) {
  RETURN_IF_UNSEALED(ctx);
  RETURN_IF_LOCKED(ctx);
  return bg_password_decrypt(password, ctx->cryptor, ctx->secret_key);
}

int bgctx_remove_password(bg_context *ctx, bg_string *name) {
  RETURN_IF_UNSEALED(ctx);
  RETURN_IF_LOCKED(ctx);
  return bg_repository_remove(ctx->repository, name);
}

int bgctx_register_memory(bg_context *ctx, bg_string *key, void *mem, void (*mem_free)(void *)) {
  return bg_map_register_data(ctx->map, key, mem, mem_free);
}

void *bgctx_get_memory(bg_context *ctx, bg_string *key) {
  return bg_map_get_data(ctx->map, key);
}
