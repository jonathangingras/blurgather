#include <stdio.h>
#include "blur.h"
#include "cmd.h"

static const char *option_strs[] = {
  "-s",
  "-f",
};

static blur_option options[] = {
  blur_unlock_from_stdin,
  blur_persistence_filepath,
};

#define NB_OPTIONS sizeof(options)/sizeof(blur_cmd)

int run_options(bg_context *ctx, int argc, char **argv) {
  int err = 0;
  size_t i;

  for(i = 0; i < argc; ++i) {
    size_t idx;

    idx = find_string_index(NB_OPTIONS, option_strs, argv[i]);

    if(idx < NB_OPTIONS) {
      if((err = options[idx](ctx, argc, argv))) {
        fprintf(stderr, "option failed! (err: %d)\n", err);
        return err;
      }
    }
  }

  return err;
}
