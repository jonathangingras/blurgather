#include <stdio.h>
#include "blur.h"
#include "cmd.h"

static const char *cmd_strs[] = {
  "get",
  "info",
  "list",
  "add",
  "remove",
};

static blur_cmd cmd_fcts[] = {
  blur_cmd_get,
  blur_cmd_info,
  blur_cmd_list,
  blur_cmd_add,
  blur_cmd_remove,
};

#define NB_CMDS sizeof(cmd_fcts)/sizeof(blur_cmd)

static int command_not_found(bg_context *ctx, int argc, char **argv) {
  fprintf(stderr, "bad usage: command not found!\n");
  return 1;
}

int run_command(bg_context *ctx, int argc, char **argv) {
  if(argc < 2) { return command_not_found; }

  size_t idx = NB_CMDS;
  size_t i = 1;
  while(idx >= NB_CMDS && i < argc) {
    idx = find_string_index(NB_CMDS, cmd_strs, argv[i]);
    ++i;
  }

  if(idx < NB_CMDS) {
    return cmd_fcts[idx](ctx, argc, argv);
  } else {
    return command_not_found(ctx, argc, argv);
  }
}
