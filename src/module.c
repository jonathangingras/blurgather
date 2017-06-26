#include <dlfcn.h>
#include <stdio.h>
#include "../include/blurgather/string.h"
#include "../include/blurgather/context.h"


#define BG_EXPORT_SYMBOL exports
#define STR(x) #x


struct bg_module_exports {
  void *(* init)();
  void (* deinit)(void *);
};
typedef struct bg_module_exports bg_module_exports;

struct bg_module {
  void *handle;

  bg_module_exports *exports;

  bg_string *name;
  bg_string *type;
  bg_string *location;
};
typedef struct bg_module bg_module;


int bg_module_open(const bg_string *path, bg_module **module) {
  void *handle = dlopen(bg_string_data(path), RTLD_LAZY);
  if(!handle) {
    fprintf(stderr, "could not load module: %s\n", dlerror());
    return -1;
  }

  void *symbol = dlsym(handle, STR(BG_EXPORT_SYMBOL));
  if(!symbol) {
    fprintf(stderr, "could not load symbol \"%s\" in module %s\n", STR(BG_EXPORT_SYMBOL), bg_string_data(path));
    dlclose(handle);
    return -2;
  }

  return 0;
}
