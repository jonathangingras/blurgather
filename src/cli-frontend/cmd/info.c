#include <stdio.h>
#include <blurgather/context.h>
#include <blurgather/password.h>
#include <blurgather/repository.h>
#include "../blur.h"
#include "../clipboard.h"


int blur_cmd_info(bg_context* ctx, int argc, char **argv) {
  printf("number of passwords: %zu\n", bg_repository_count(bgctx_repository(ctx)));

  return 0;
}
