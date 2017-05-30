#ifndef BLURGATHER_PERSISTER_H
#define BLURGATHER_PERSISTER_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_persister_vtable {
  void (* const destroy)(bg_persister_t *self);
  int (* const load)(bg_persister_t *self);
  int (* const persist)(bg_persister_t *self);
};

struct bg_persister_t {
  const struct bg_persister_vtable *vtable;
  const void *object;
};

void bg_persister_destroy(bg_persister_t *self);
int bg_persister_load(bg_persister_t *self);
int bg_persister_persist(bg_persister_t *self);

#ifdef __cplusplus
}
#endif

#endif
