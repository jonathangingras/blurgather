#ifndef _KIKI_MSGPACK_SERIALIZE_H_
#define _KIKI_MSGPACK_SERIALIZE_H_

#include <blurgather/password.h>
#include <blurgather/msgpack_persister.h>
#include <msgpack.h>

#ifdef __cplusplus
extern "C" {
#endif

int bg_persistence_msgpack_serialize_password(msgpack_packer* packer, bg_password* password);
int bg_persistence_msgpack_deserialize_password(msgpack_object* object, bg_password* password);

int bg_persistence_msgpack_serialize_password_array(bg_msgpack_persister* self,
                                                     msgpack_sbuffer* buffer, bg_repository_t *repo);
int bg_persistence_msgpack_deserialize_password_array(bg_msgpack_persister* self,
                                                      unsigned char* data, size_t length, bg_repository_t *repo);

#ifdef __cplusplus
}
#endif

#endif
