#include <stdio.h>
#include <string.h>
#include "blur.h"
#include "cmd.h"
#include <blurgather/mcrypt_cryptor.h>
#include <blurgather/array_repository.h>
#include <blurgather/msgpack_persister.h>


typedef int (* const blur_cmd)(bg_context *ctx, int argc, char **argv);

static const char *cmd_strs[] = {
  "get",
  "info",
  "list",
  "add",
  "remove"
};

static blur_cmd cmd_fcts[] = {
  blur_cmd_get,
  blur_cmd_info,
  blur_cmd_list,
  blur_cmd_add,
  blur_cmd_remove
};

static int command_not_found(bg_context *ctx, int argc, char **argv) {
  fprintf(stderr, "bad usage: command not found!\n");
  return 1;
}

static blur_cmd find_cmd_function(const char* cmd) {
  size_t i;
  for(i = 0; i < sizeof(cmd_strs)/sizeof(char *); ++i) {
    if(strcmp(cmd_strs[i], cmd) == 0) {
      return cmd_fcts[i];
    }
  }

  return command_not_found;
}

bg_string *default_persistence_filepath() {
  char *home = getenv("HOME");
  char *rest =  "/.blrgthrc";
  bg_string *repo_filepath = bg_string_from_str(home);
  bg_string_cat_char_array(&repo_filepath, rest, strlen(rest));
  return repo_filepath;
}

int main(int argc, char **argv) {
  int err = 0;
  bg_context *ctx = NULL;

  if(argc < 2) {
    fprintf(stderr, "bad usage!\n");
    return 1;
  }

  if((err = blur_create_context(&ctx,
                                bg_msgpack_persister_persister(bg_msgpack_persister_new(default_persistence_filepath())),
                                bg_password_array_repository_new(),
                                bg_mcrypt_cryptor()))) {
    fprintf(stderr, "context could not be instantiated!\n");
    return err;
  }

  err = find_cmd_function(argv[1])(ctx, argc, argv);

  bgctx_finalize(ctx);

  return err;
}
