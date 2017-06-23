#include <stdio.h>
#include <blurgather/context.h>
#include <blurgather/password.h>
#include "../blur.h"


bg_string *get_or_ask_field(const char *field, int argc, char **argv, int hide_input, int *was_interactive) {
  char option[2 + strlen(field) + 1];
  option[0] = 0;
  strcat(option, "--");
  strcat(option, field);
  option[sizeof(option) - 1] = 0;

  size_t idx = find_string_index(argc, argv, option);

  if(idx + 1 < argc) {
    if(was_interactive) {
      *was_interactive = 0;
    }

    if(argv[idx + 1][0] != '-') { return bg_string_from_str(argv[idx + 1]); }
    else { return bg_string_new(); }
  } else {
    if(was_interactive) {
      *was_interactive = 1;
    }
    return blur_getfield(field, hide_input);
  }
}


int blur_cmd_add(bg_context* ctx, int argc, char **argv) {
  int return_value = 0;

  bg_password* password = bg_password_new();

  bg_string *name = get_or_ask_field("name", argc, argv, 0, NULL);
  bg_string *desc = get_or_ask_field("description", argc, argv, 0, NULL);

  bg_string *value1 = NULL;
  bg_string *value2 = NULL;
  int interactive;
  value1 = get_or_ask_field("value", argc, argv, 1, &interactive);
  if(interactive) {
    value2 = blur_getfield("confirmation", 1);
  }

  if(value2 && bg_string_compare(value1, value2)) {
    fprintf(stderr, "password values do not match!\n");
    bg_string_clean_free(value1);
    bg_string_clean_free(value2);
    return -1;
  }
  if(value2) { bg_string_clean_free(value2); }

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
