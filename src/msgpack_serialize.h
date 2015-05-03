#ifndef _KIKI_MSGPACK_SERIALIZE_H_
#define _KIKI_MSGPACK_SERIALIZE_H_

#include <msgpack.h>
#include <kiki/utilities.h>
#include <kiki/pwd_mng/kiPassword.h>
#include <kiki/pwd_mng/kiPasswordFactory.h>
#include <kiki/pwd_mng/kiPasswordMsgPackPersister.h>

#ifdef __cplusplus
extern "C" {
#endif

void kiki_pwd_mng_Persistance_msgpack_serialize_kiPassword(msgpack_packer* packer, kiPassword* password);
int kiki_pwd_mng_Persistance_msgpack_deserialize_kiPassword(msgpack_object* object, kiPassword* password);

void kiki_pwd_mng_Persistance_msgpack_serialize_kiPasswordArray(kiPasswordMsgPackPersister* self,
                                                                msgpack_sbuffer* buffer);
int kiki_pwd_mng_Persistance_msgpack_deserialize_kiPasswordArray(kiPasswordMsgPackPersister* self,
                                                                 kiPasswordFactory* factory,
                                                                 unsigned char* data, size_t length);

#ifdef __cplusplus
}
#endif

#endif