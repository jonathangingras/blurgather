#include <blurgather/persister.h>


void bg_persister_destroy(bg_persister_t *self) {
  self->vtable->destroy(self);
}

int bg_persister_load(bg_persister_t *self, bg_repository_t *repo) {
  return self->vtable->load(self, repo);
}

int bg_persister_persist(bg_persister_t *self, bg_repository_t *repo) {
  return self->vtable->persist(self, repo);
}
