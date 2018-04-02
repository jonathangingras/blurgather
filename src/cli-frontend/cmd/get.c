#include <stdio.h>
#include <blurgather/context.h>
#include <blurgather/password.h>
#include "../blur.h"
#include "../clipboard.h"


int blur_cmd_get(bg_context* ctx, int argc, char **argv) {
  int err = 0;
  bg_string *name = NULL;
  bg_password *password, *password_copy;
  void (*send_)(const char*) = NULL;
  void (*clear_)(void);
  size_t n_passwords_to_get = 1;

  send_ = bgctx_get_memory(ctx, bg_string_from_str("clipboard"));
  clear_ = bgctx_get_memory(ctx, bg_string_from_str("clear_clipboard"));

  if(!send_) {
    fprintf(stderr, "cannot send value, function pointer is null!\n");
    return -2;
  }
  if(!clear_) {
    fprintf(stderr, "function clipboard clearing pointer is null!\n");
  }

  size_t get_idx = find_string_index(argc, (const char **)argv, "get");
  size_t arg_idx = get_idx + 1;

  if(get_idx == ((size_t)argc) - 1) {
    name = blur_getfield("name", 0);
  } else if(get_idx < ((size_t)argc) - 1) {
    n_passwords_to_get = ((size_t)argc) - 1 - get_idx;
    name = bg_string_from_str(argv[arg_idx]);
  } else {
    fprintf(stderr, "bad usage!\n");
    return -1;
  }

  while(arg_idx < (size_t)argc) {
    if((err = bgctx_find_password(ctx, name, &password))) {
      fprintf(stderr, "could not find password!\n");
      bg_string_free(name);
      return err;
    }

    password_copy = bg_password_copy(password);
    if((err = bgctx_decrypt_password(ctx, password_copy))) {
      fprintf(stderr, "could not decrypt password!\n");
      bg_password_free(password_copy);
      bg_string_free(name);
      return err;
    }

    send_(bg_string_data(bg_password_value(password_copy)));
    if(arg_idx < (size_t)argc) {
      getchar();
    }

    bg_password_free(password_copy);
    bg_string_free(name);

    ++arg_idx;
    if(arg_idx < (size_t)argc) {
      name = bg_string_from_str(argv[arg_idx]);
    }
  }

  if(clear_) {
    clear_();
  }

  return err;
}
