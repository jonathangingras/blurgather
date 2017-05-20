#include "msgpack_serialize.h"
#include <blurgather/repository.h>

static int get_keyvalue_iterator(const char* field, msgpack_object_kv** keyvalue_iterator_ptr, char** iterator, size_t* size, int error_value) {
  if(memcmp(field, (*keyvalue_iterator_ptr)->key.via.str.ptr, (*keyvalue_iterator_ptr)->key.via.str.size)) {
    return error_value;
  }
  *iterator = (char*)(*keyvalue_iterator_ptr)->val.via.str.ptr;
  *size = (*keyvalue_iterator_ptr)->val.via.str.size;

  return 0;
}

void bg_persistence_msgpack_serialize_password(msgpack_packer* packer, bg_password* password) {
  msgpack_pack_map(packer, 4);

  msgpack_pack_str(packer, 4);
  msgpack_pack_str_body(packer, "name", 4);
  size_t name_len = bg_string_length(bg_password_name(password));
  msgpack_pack_str(packer, name_len);
  msgpack_pack_str_body(packer, bg_string_data(bg_password_name(password)), name_len);

  const bg_iv_t *iv = bg_password_iv(password);
  msgpack_pack_str(packer, 2);
  msgpack_pack_str_body(packer, "iv", 2);
  msgpack_pack_bin(packer, bg_iv_length(iv));
  msgpack_pack_bin_body(packer, bg_iv_data(iv), bg_iv_length(iv));

  msgpack_pack_str(packer, 11);
  msgpack_pack_str_body(packer, "description", 11);
  size_t description_len = bg_string_length(bg_password_description(password));
  msgpack_pack_str(packer, description_len);
  msgpack_pack_str_body(packer, bg_string_data(bg_password_description(password)), description_len);

  msgpack_pack_str(packer, 5);
  msgpack_pack_str_body(packer, "value", 5);
  size_t value_len = bg_password_value_length(password);
  msgpack_pack_bin(packer, value_len);
  msgpack_pack_bin_body(packer, bg_string_data(bg_password_value(password)), value_len);
}

int bg_persistence_msgpack_deserialize_password(msgpack_object* object, bg_password* password) {
  msgpack_object_kv* keyvalue_iterator = object->via.map.ptr;
  int error_value = 0;
  char * name_iterator, * description_iterator;
  unsigned char * iv_iterator, * value_iterator;
  size_t iv_size, name_size, description_size, value_size;

  if((error_value = get_keyvalue_iterator("name", &keyvalue_iterator, &name_iterator, &name_size,
                                          -3))) { return error_value; }
  ++keyvalue_iterator;
  if((error_value = get_keyvalue_iterator("iv", &keyvalue_iterator, (char **)&iv_iterator, &iv_size,
                                          -2))) { return error_value; }
  ++keyvalue_iterator;
  if((error_value = get_keyvalue_iterator("description", &keyvalue_iterator, &description_iterator,
                                          &description_size, -4))) { return error_value; }
  ++keyvalue_iterator;
  if((error_value = get_keyvalue_iterator("value", &keyvalue_iterator, (char **)&value_iterator, &value_size,
                                          -5))) { return error_value; }


  bg_iv_t *iv = bg_iv_new(bg_password_ctx(password), iv_iterator, iv_size);
  if(bg_password_fill_raw(password, iv, value_iterator, value_size)) {
    return -1;
  }

  bg_string *name_buffer = bg_string_from_char_array(name_iterator, name_size);
  bg_password_update_name(password, name_buffer);
  bg_string_free(name_buffer);

  bg_string *desc_buffer = bg_string_from_char_array(description_iterator, description_size);
  bg_password_update_description(password, desc_buffer);
  bg_string_free(desc_buffer);

  return error_value;
}

struct serialize_data {
  msgpack_packer *packer;
  bg_msgpack_persister *persister;
};

static int serialize_password(bg_password *pwd, void *_serialize_data) {
  msgpack_packer *pk = ((struct serialize_data *)_serialize_data)->packer;
  bg_msgpack_persister *self = ((struct serialize_data *)_serialize_data)->persister;

  bg_persistence_msgpack_serialize_password(pk, pwd);
  return 0;
}

void bg_persistence_msgpack_serialize_password_array(bg_msgpack_persister* self, msgpack_sbuffer* buffer) {
  msgpack_packer pk;
  msgpack_packer_init(&pk, buffer, msgpack_sbuffer_write);

  bg_repository_t *repo = bgctx_repository(self->ctx);
  if(!repo) { return; }

  msgpack_pack_array(&pk, bg_repository_count(repo));

  struct serialize_data data = {
    .packer = &pk,
    .persister = self,
  };

  bg_repository_foreach(repo, &serialize_password, &data);

  /*int i;
  for(i = 0; i < self->number_passwords; ++i) {
    bg_persistence_msgpack_serialize_password(&pk, self->password_array[i]);
    }*/
}

int bg_persistence_msgpack_deserialize_password_array(bg_msgpack_persister* self, unsigned char* data, size_t data_length) {
  msgpack_zone mempool;
  msgpack_zone_init(&mempool, 2048);
  msgpack_object deserialized;
  msgpack_unpack((char*)data, data_length, NULL, &mempool, &deserialized);

  int i, error_value;
  msgpack_object* ptr = deserialized.via.array.ptr;
  for(i = 0; i < deserialized.via.array.size; ++i) {
    bg_password* password = bg_password_new(self->ctx);
    error_value = bg_persistence_msgpack_deserialize_password(ptr + i, password);
    if(error_value) { return error_value; }
    error_value = bg_password_save(password);
    if(error_value) { return error_value; }
  }

  msgpack_zone_destroy(&mempool);
  return 0;
}
