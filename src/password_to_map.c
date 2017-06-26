#include <stdio.h>
#include <blurgather/password.h>
#include <blurgather/map.h>
#include <blurgather/password_to_map.h>


#define REGISTER_FIELD(map, field)                                      \
  if((err = bg_map_register_data(map,                                   \
                                 bg_string_from_str(#field),            \
                                 bg_string_copy(                        \
                                   bg_password_##field(password)),      \
                                 bg_string_free))) {                    \
    fprintf(stderr,                                                     \
            "could not register field \"%s\", (err: %d)!\n",            \
            #field, err);                                               \
    return NULL;                                                        \
  }


bg_map *bg_password_to_map(const bg_password *password) {
  bg_map *map = bg_map_new();
  int err;

  REGISTER_FIELD(map, name);
  REGISTER_FIELD(map, description);
  REGISTER_FIELD(map, value);

  return map;
}
