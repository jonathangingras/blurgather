#include <stdio.h>
#include <string.h>
#include "../blur.h"
#include "../clipboard.h"


int blur_value_to_stdout(bg_context *ctx, int argc, char **argv) {
  int err = 0;

  if((err = bgctx_register_memory(ctx,
                                  bg_string_from_str("clipboard"),
                                  &send_to_stdout, NULL))) {
    fprintf(stderr, "could not register stdout as clipboard!\n");
  }

  return err;
}
