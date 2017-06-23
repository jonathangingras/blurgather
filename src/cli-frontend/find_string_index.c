#include "blur.h"

size_t find_string_index(int argc, const char** argv, const char *str) {
  size_t i;
  for(i = 0; i < argc; ++i) {
    if(strcmp(argv[i], str) == 0) {
      return i;
    }
  }

  return argc;
}
