#include <blurgather/repository.h>

void bg_repository_destroy(bg_repository_t *self) {
  self->vtable->destroy(self);
}

void bg_repository_free(bg_repository_t *self) {
  self->vtable->destroy(self);
  free((void*) self->object);
}

int bg_repository_add(bg_repository_t *self, bg_password* password) {
  return self->vtable->add(self, password);
}

int bg_repository_get(bg_repository_t *self, const bg_string *name, bg_password **password) {
  return self->vtable->get(self, name, password);
}

int bg_repository_remove(bg_repository_t *self, const bg_string *name) {
  return self->vtable->remove(self, name);
}

size_t bg_repository_count(bg_repository_t *self) {
  return self->vtable->count(self);
}

int bg_repository_foreach(bg_repository_t *self, int (* callback)(bg_password *, void *), void *output) {
  return self->vtable->foreach(self, callback, output);
}
