#include <stdio.h>
#include <blurgather/context.h>
#include <blurgather/password.h>
#include "../blur.h"
#include "../clipboard.h"


int blur_cmd_get(bg_context* ctx, int argc, char **argv) {
  int err = 0;
  bg_string *name = NULL;

  if((err = bgctx_load(ctx))) {
    fprintf(stderr, "could not load repository!\n");
    return err;
  }

  if(argc == 2) {
    name = blur_getfield("name", 0);
  } else if(argc == 3) {
    name = bg_string_from_str(argv[2]);
  } else {
    fprintf(stderr, "bad usage!\n");
    return -1;
  }

  bg_password *password;
  if((err = bgctx_find_password(ctx, name, &password))) {
    fprintf(stderr, "could not find password!\n");
    bg_string_free(name);
    return err;
  }

  if((err = bgctx_unlock(ctx, blur_get_secret_key(ctx)))) {
    fprintf(stderr, "could not unlock context!\n");
    return err;
  } else {
    if((err = bgctx_decrypt_password(ctx, password))) {
      fprintf(stderr, "could not decrypt password!\n");
      bg_string_free(name);
      return err;
    }
    send_to_clipboard(bg_string_data(bg_password_value(password)));
    bgctx_encrypt_password(ctx, password);
  }
  bgctx_lock(ctx);

  bg_string_free(name);
  return err;
}
