#include <blurgather/repository.h>

void bg_repository_destroy(bg_repository_t *self) {
  self->vtable->destroy(self);
}

int bg_repository_add(bg_repository_t *self, bg_password* password) {
  return self->vtable->add(self, password);
}

int bg_repository_get(bg_repository_t *self, const bg_string *name, bg_password **password) {
  return 1;
}

int bg_repository_remove(bg_repository_t *self, const bg_string *name) {
  return self->vtable->remove(self, name);
}

void bg_repository_sort(bg_repository_t *self) {
  self->vtable->sort(self);
}

bg_password_iterator bg_repository_begin(bg_repository_t *self) {
  return self->vtable->begin(self);
}

bg_password_iterator bg_repository_end(bg_repository_t *self) {
  return self->vtable->end(self);
}

int bg_repository_load(bg_repository_t *self) {
  return self->vtable->load(self);
}

int bg_repository_persist(bg_repository_t *self) {
  return self->vtable->persist(self);
}

int bg_repository_foreach(bg_repository_t *self, int (* callback)(bg_password *, void *), void *output) {
  bg_password_iterator iterator = bg_repository_begin(self);
  bg_password_iterator end = bg_repository_end(self);

  for(; iterator.value != end.value; iterator.next(&iterator)) {
    if(callback(*iterator.value, output)) {
      return 1;
    }
  }

  return 0;
}
