#include <stdio.h>
#include <blurgather/context.h>
#include <blurgather/password.h>

static int print_password_name(bg_password *pwd, bg_context *ctx) {
  bg_password *copy = bg_password_copy(pwd);

  int err = 0;
  if((err = bg_password_decrypt(copy, bgctx_cryptor(ctx), bgctx_access_key(ctx)))) { /* decrypt only the copy */
    bg_password_free(copy);
    return err;
  }

  printf("%s\n", bg_string_data(bg_password_name(copy)));

  bg_password_free(copy);
  return 0;
}

int blur_cmd_list(bg_context *ctx, int argc, char **argv) {
  return bgctx_each_password(ctx, (int(*)(bg_password *, void *))&print_password_name, ctx);
}
