#include <stdio.h>
#include <string.h>
#include "blur.h"
#include <blurgather/mcrypt_cryptor.h>
#include <blurgather/array_repository.h>
#include <blurgather/msgpack_persister.h>
#include "blur.h"


bg_string *default_persistence_filepath() {
  char *home = getenv("HOME");
  char *rest =  "/.blrgthrc";
  bg_string *repo_filepath = bg_string_from_str(home);
  bg_string_cat_char_array(&repo_filepath, rest, strlen(rest));
  return repo_filepath;
}

int end(bg_context *ctx, int err) {
  bgctx_finalize(ctx);
  return err;
}

int main(int argc, char **argv) {
  int err = 0;
  bg_context *ctx = NULL;

  if(argc < 2) {
    fprintf(stderr, "bad usage!\n");
    return 1;
  }

  if((err = blur_create_context(&ctx,
                                bg_msgpack_persister_persister(
                                  bg_msgpack_persister_new(
                                    default_persistence_filepath())),
                                bg_password_array_repository_new(),
                                bg_mcrypt_cryptor()))) {
    fprintf(stderr, "context could not be instantiated!\n");
    return err;
  }

  if((err = run_options(ctx, argc, argv))) {
    fprintf(stderr, "running options failed! (err: %d)\n", err);
    return end(ctx, err);
  }

  if(bgctx_locked(ctx)) {
    if((err = bgctx_unlock(ctx, blur_ask_secret_key(ctx)))) {
      fprintf(stderr, "could not unlock context!\n");
      return end(ctx, err);
    }
  }

  if((err = run_command(ctx, argc, argv))) {
    fprintf(stderr, "running command failed! (err: %d)\n", err);
    return end(ctx, err);
  }

  return end(ctx, err);
}
