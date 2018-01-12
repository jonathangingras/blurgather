#ifndef BLURGATHER_MSGPACK_PERSISTER_H
#define BLURGATHER_MSGPACK_PERSISTER_H

#include "persister.h"
#include "string.h"
#include "secret_key.h"
#include "cryptor.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_msgpack_persister;
typedef struct bg_msgpack_persister bg_msgpack_persister;

struct bg_msgpack_persister {
  bg_persister_t persister;

  bg_string *persistence_filename;

  bg_cryptor_t *cryptor;
  bg_secret_key_t *secret_key;
};

bg_msgpack_persister *bg_msgpack_persister_new(bg_string *filename, bg_cryptor_t *cryptor);

bg_persister_t *bg_msgpack_persister_persister(bg_msgpack_persister *persister);

int bg_msgpack_persister_register_key(bg_persister_t *self, bg_secret_key_t *secret_key);

int bg_msgpack_persister_unregister_key(bg_persister_t *self);

#ifdef __cplusplus
}
#endif

#endif
