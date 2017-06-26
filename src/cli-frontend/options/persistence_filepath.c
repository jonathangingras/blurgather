#include <stdio.h>
#include "../blur.h"

static int error_no_argument() {
  fprintf(stderr, "-f cannot be used without filepath argument!\n");
  return -1;
}

int blur_persistence_filepath(bg_context *ctx, int argc, char **argv) {
  int err = 0;
  size_t f_idx = find_string_index(argc, argv, "-f");

  if(f_idx + 1 < argc) {
    if(argv[f_idx + 1][0] != '-') {
      if((err = bgctx_register_memory(ctx,
                                      bg_string_from_str("persistence_filepath"),
                                      bg_string_from_str(argv[f_idx + 1]),
                                      bg_string_free))) {
        fprintf(stderr, "could not register memory to filepath!\n");
        return err;
      }
    } else {
      return error_no_argument();
    }
  } else {
    return error_no_argument();
  }

  return 0;
}
