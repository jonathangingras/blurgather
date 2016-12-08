#ifndef BLURGATHER_PASSWORD_REPOSITORY_H
#define BLURGATHER_PASSWORD_REPOSITORY_H

#include "password.h"
#include "password_iterator.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_repository_vtable {
  void (* const destroy)(bg_repository_t *self);

  int (* const add)(bg_repository_t *self, bg_password *password);
  int (* const get)(bg_repository_t *self, const bg_string *name, bg_password **password);
  int (* const remove)(bg_repository_t *self, const bg_string *name);

  void (* const sort)(bg_repository_t *self);
  bg_password_iterator (* const begin)(bg_repository_t *self);
  bg_password_iterator (* const end)(bg_repository_t *self);

  int (* const load)(bg_repository_t *self);
  int (* const persist)(bg_repository_t *self);
};

struct bg_repository_t {
  const void *object;
  const struct bg_repository_vtable *vtable;
};

void bg_repository_destroy(bg_repository_t *self);

int bg_repository_add(bg_repository_t *self, bg_password *password);
int bg_repository_get(bg_repository_t *self, const bg_string *name, bg_password **password);

int bg_repository_remove(bg_repository_t *self, const bg_string *name);
void bg_repository_sort(bg_repository_t *self);

bg_password_iterator bg_repository_begin(bg_repository_t *self);
bg_password_iterator bg_repository_end(bg_repository_t *self);

int bg_repository_load(bg_repository_t *self);
int bg_repository_persist(bg_repository_t *self);

int bg_repository_foreach(bg_repository_t *self, int (* callback)(bg_password *, void *), void *output);

#ifdef __cplusplus
}
#endif

#endif /* BLURGATHER_PASSWORD_REPOSITORY_H */
