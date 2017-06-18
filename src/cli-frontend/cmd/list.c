#include <stdio.h>
#include <blurgather/context.h>
#include <blurgather/password.h>

static int print_password(bg_password *pwd, void *_) {
  return printf("%s\n", bg_string_data(bg_password_name(pwd)));
}

int blur_cmd_list(bg_context *ctx, int argc, char **argv) {
  return bgctx_each_password(ctx, &print_password, NULL);
}
