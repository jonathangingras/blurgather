#ifndef BLURGATHER_MSGPACK_PERSISTER_H
#define BLURGATHER_MSGPACK_PERSISTER_H

#include "persister.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_msgpack_persister;
typedef struct bg_msgpack_persister bg_msgpack_persister;

struct bg_msgpack_persister {
  bg_persister_t persister;

  bg_context *ctx;

  bg_string *persistence_filename;
};

bg_msgpack_persister *bg_msgpack_persister_new(bg_context *ctx, bg_string *filename);

bg_persister_t *bg_msgpack_persister_persister(bg_msgpack_persister *persister);

#ifdef __cplusplus
}
#endif

#endif
