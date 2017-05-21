#include <blurgather/persister.h>
#include <stdlib.h>

void bg_persister_destroy(bg_persister_t *self) {
  self->vtable->destroy(self);
}

void bg_persister_free(bg_persister_t *self) {
  self->vtable->destroy(self);
  free((void*)self->object);
}

int bg_persister_load(bg_persister_t *self) {
  return self->vtable->load(self);
}

int bg_persister_persist(bg_persister_t *self) {
  return self->vtable->persist(self);
}
