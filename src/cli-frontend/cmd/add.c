#include <stdio.h>
#include <blurgather/context.h>
#include <blurgather/password.h>
#include "../blur.h"


int blur_cmd_add(bg_context* ctx, int argc, char **argv) {
  int return_value = 0;

  bg_password* password = bg_password_new();

  bg_string *name = blur_getfield("name", 0);
  bg_string *desc = blur_getfield("description", 0);
  bg_string *value1 = blur_getfield("value", 1);
  bg_string *value2 = blur_getfield("confirmation", 1);

  if(bg_string_compare(value1, value2)) {
    fprintf(stderr, "password values do not match!\n");
    bg_string_clean_free(value1);
    bg_string_clean_free(value2);
    return -1;
  }
  bg_string_clean_free(value2);

  bg_password_update_name(password, name);
  bg_password_update_description(password, desc);
  bg_password_update_value(password, value1, bgctx_cryptor(ctx));

  if((return_value = bgctx_encrypt_password(ctx, password))) {
    fprintf(stderr, "password encryption failed!\n");
    bg_password_free(password);
    return return_value;
  }

  if((return_value = bgctx_add_password(ctx, password))) {
    fprintf(stderr, "adding password failed!\n");
    bg_password_free(password);
    return return_value;
  }

  if((return_value = bgctx_persist(ctx))) {
    fprintf(stderr, "persistence failed!\n");
    return return_value;
  }

  return return_value;
}
