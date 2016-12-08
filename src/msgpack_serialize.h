#ifndef _KIKI_MSGPACK_SERIALIZE_H_
#define _KIKI_MSGPACK_SERIALIZE_H_

#include <msgpack.h>
#include "blurgather/password.h"
#include "blurgather/password_msgpack_persister.h"

#ifdef __cplusplus
extern "C" {
#endif

void bg_persistence_msgpack_serialize_password(msgpack_packer* packer, bg_password* password);
int bg_persistence_msgpack_deserialize_password(msgpack_object* object, bg_password* password);

void bg_persistence_msgpack_serialize_password_array(bg_password_msgpack_persister* self,
                                                                msgpack_sbuffer* buffer);
int bg_persistence_msgpack_deserialize_password_array(bg_password_msgpack_persister* self,
                                                                 unsigned char* data, size_t length);

#ifdef __cplusplus
}
#endif

#endif
