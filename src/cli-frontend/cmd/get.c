#include <stdio.h>
#include <blurgather/context.h>
#include <blurgather/password.h>
#include "../blur.h"
#include "../clipboard.h"


int blur_cmd_get(bg_context* ctx, int argc, char **argv) {
  int err = 0;
  bg_string *name = NULL;
  bg_password *password;
  void (*send_)(const char*) = NULL;
  send_ = bgctx_get_memory(ctx, bg_string_from_str("clipboard"));

  if(!send_) {
    fprintf(stderr, "cannot send value, function pointer is null!\n");
    return -2;
  }

  size_t get_idx = find_string_index(argc, argv, "get");

  if(get_idx == argc - 1) {
    name = blur_getfield("name", 0);
  } else if(get_idx < argc - 1) {
    name = bg_string_from_str(argv[get_idx + 1]);
  } else {
    fprintf(stderr, "bad usage!\n");
    return -1;
  }

  if((err = bgctx_find_password(ctx, name, &password))) { /* returns a decrypted copy */
    fprintf(stderr, "could not find password!\n");
    bg_string_free(name);
    return err;
  }

  send_(bg_string_data(bg_password_value(password)));
  bg_password_free(password); /* clean and free the copy */

  bg_string_free(name);
  return err;
}
