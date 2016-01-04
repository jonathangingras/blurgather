#ifndef BLURGATHER_PASSWORD_REPOSITORY_H
#define BLURGATHER_PASSWORD_REPOSITORY_H

#ifdef __cplusplus
extern "C" {
#endif

struct bg_password_repository;
typedef struct bg_password_repository bg_password_repository;

#include "password.h"
#include "password_iterator.h"

struct bg_password_repository_vtable {
  void (* destroy)(bg_password_repository* self);
  
  int (* add)(bg_password_repository* self, bg_password* password);
  int (* remove)(bg_password_repository* self, const char* name);
  
  void (* sort)(bg_password_repository* self);
  bg_password_iterator (* begin)(bg_password_repository* self);
  bg_password_iterator (* end)(bg_password_repository* self);
  
  int (* load)(bg_password_repository* self);
  int (* persist)(bg_password_repository* self);
};

struct bg_password_repository {
  const void *object;
  const struct bg_password_repository_vtable *vtable;
};

void bg_pwd_repository_destroy(bg_password_repository* self);

int bg_pwd_repository_add(bg_password_repository* self, bg_password* password);
int bg_pwd_repository_remove(bg_password_repository* self, const char* name);
void bg_pwd_repository_sort(bg_password_repository* self);

bg_password_iterator bg_pwd_repository_begin(bg_password_repository* self);
bg_password_iterator bg_pwd_repository_end(bg_password_repository* self);

int bg_pwd_repository_load(bg_password_repository* self);
int bg_pwd_repository_persist(bg_password_repository* self);

#ifdef __cplusplus
}
#endif

#endif //BLURGATHER_PASSWORD_REPOSITORY_H
