#include <stdio.h>
#include <string.h>
#include <blurgather/mcrypt_cryptor.h>
#include <blurgather/array_repository.h>
#include <blurgather/msgpack_persister.h>
#include "blur.h"
#include "clipboard.h"


bg_string *default_persistence_filepath() {
  char *home = getenv("HOME");
  char *rest =  "/.blurdb";
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

  if((err = bgctx_init(&ctx))) {
    fprintf(stderr, "could not instantiate blurgather context: %d\n", err);
    return err;
  }

  bgctx_register_memory(ctx, bg_string_from_str("persistence_filepath"),
                        default_persistence_filepath(), bg_string_free);
  bgctx_register_memory(ctx, bg_string_from_str("clipboard"), &send_to_clipboard, NULL);
  bgctx_register_memory(ctx, bg_string_from_str("clear_clipboard"), &clear_clipboard, NULL);

  if((err = run_options(ctx, argc, argv))) {
    fprintf(stderr, "running options failed! (err: %d)\n", err);
    return end(ctx, err);
  }

  bg_cryptor_t *cryptor = bg_mcrypt_cryptor();
  bg_persister_t *persister = bg_msgpack_persister_persister(bg_msgpack_persister_new(
                                                               bg_string_copy(bgctx_get_memory(ctx, bg_string_from_str("persistence_filepath"))),
                                                               cryptor));
  if((err = blur_setup_context(ctx,
                               persister,
                               bg_password_array_repository_new(),
                               cryptor))) {
    fprintf(stderr, "context could not be instantiated!\n");
    return err;
  }

  if(bgctx_locked(ctx)) {
    if((err = bgctx_unlock(ctx, blur_ask_secret_key(ctx)))) {
      fprintf(stderr, "could not unlock context!\n");
      return end(ctx, err);
    }
  }

  if(bg_msgpack_persister_register_key(persister, bgctx_access_key(ctx))) {
    fprintf(stderr, "could not register secret key to persister!\n");
    return end(ctx, err);
  }

  if((err = bgctx_load(ctx))) {
    if(err == -4) {
      fprintf(stderr, "could not load repository (err %d), creating empty one.\n", err);

      if((err = bgctx_persist(ctx))) {
        fprintf(stderr, "could not persist repository!\n");
        return err;
      }
    } else {
      fprintf(stderr, "loading repository failed (err %d)!\n", err);
      return err;
    }
  }

  if((err = run_command(ctx, argc, argv))) {
    fprintf(stderr, "running command failed! (err: %d)\n", err);
    return end(ctx, err);
  }

  return end(ctx, err);
}
