#include <stdio.h>
#include <blurgather/context.h>
#include <blurgather/password.h>
#include "../blur.h"

int blur_cmd_remove(bg_context* ctx, int argc, char **argv) {
  int err = 0;
  bg_string *name = blur_getfield("name", 0);

  bg_string *valid_yes = bg_string_from_str("yes");
  bg_string *valid_no = bg_string_from_str("no");

  while(1) {
    bg_string *confirmation = blur_getfield("are you sure?", 0);

    if(bg_string_compare(valid_yes, confirmation) == 0) {
      if((err = bgctx_remove_password(ctx, name))) {
        fprintf(stderr, "removing password failed!\n");
      }
      bg_string_free(confirmation);
      break;
    } else if(bg_string_compare(valid_no, confirmation) == 0) {
      bg_string_free(confirmation);
      break;
    } else {
      printf("you must answer by 'yes' or 'no'\n");
      bg_string_free(confirmation);
    }
  }

  bg_string_free(name);
  bg_string_free(valid_yes);
  bg_string_free(valid_no);
  return err;
}
