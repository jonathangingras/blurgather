#ifndef BLURGATHER_PASSWORD_REPOSITORY_H
#define BLURGATHER_PASSWORD_REPOSITORY_H

#include "password.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_repository_vtable {
  void (* const destroy)(bg_repository_t *self);

  int (* const add)(bg_repository_t *self, bg_password *password);
  int (* const get)(bg_repository_t *self, const bg_string *name, bg_password **password);
  int (* const remove)(bg_repository_t *self, const bg_string *name);

  size_t (* const count)(bg_repository_t *self);
  int (* const foreach)(bg_repository_t *self, int (* callback)(bg_password *, void *), void *output);
};

struct bg_repository_t {
  const struct bg_repository_vtable *vtable;
  const void *object;
};

void bg_repository_destroy(bg_repository_t *self);

int bg_repository_add(bg_repository_t *self, bg_password *password);
int bg_repository_get(bg_repository_t *self, const bg_string *name, bg_password **password);

int bg_repository_remove(bg_repository_t *self, const bg_string *name);
size_t bg_repository_count(bg_repository_t *self);

int bg_repository_foreach(bg_repository_t *self, int (* callback)(bg_password *, void *), void *output);

#ifdef __cplusplus
}
#endif

#endif /* BLURGATHER_PASSWORD_REPOSITORY_H */
