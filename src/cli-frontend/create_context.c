#include <stdio.h>
#include <blurgather/context.h>
#include "blur.h"


int blur_setup_context(bg_context *ctx,
                        bg_persister_t *persister, bg_repository_t *repo, bg_cryptor_t *cryptor) {
  int err = 0;

  if((err = bgctx_register_persister(ctx, persister))) {
    ERROR_AND_RETURN(-2, "could not register persister: %d\n", err);
  }
  if((err = bgctx_register_repository(ctx, repo))) {
    ERROR_AND_RETURN(-3, "could not register repository: %d\n", err);
  }
  if((err = bgctx_register_cryptor(ctx, cryptor))) {
    ERROR_AND_RETURN(-4, "could not register cryptor: %d\n", err);
  }
  if((err = bgctx_config(ctx, BGCTX_ACQUIRE_PERSISTER | BGCTX_ACQUIRE_REPOSITORY))) {
    ERROR_AND_RETURN(-5, "could not configurate context: %d\n", err);
  }
  if((err = bgctx_seal(ctx))) {
    ERROR_AND_RETURN(-6, "could not seal context: %d\n", err);
  }

  return err;
}
