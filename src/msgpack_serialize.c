#include "msgpack_serialize.h"
#include <blurgather/repository.h>
#include <blurgather/map.h>
#include <blurgather/password_to_map.h>

static int get_keyvalue_iterator(const char *field, msgpack_object_kv **keyvalue_iterator_ptr, char **iterator, size_t *size, int error_value) {
  if(memcmp(field, (*keyvalue_iterator_ptr)->key.via.str.ptr, (*keyvalue_iterator_ptr)->key.via.str.size)) {
    return error_value;
  }
  *iterator = (char*)(*keyvalue_iterator_ptr)->val.via.str.ptr;
  *size = (*keyvalue_iterator_ptr)->val.via.str.size;

  return 0;
}

static int serialize_field(const bg_string *field_key,
                           const bg_string *field_value,
                           msgpack_packer *packer) {
  /* key */
  msgpack_pack_str(packer, bg_string_length(field_key));
  msgpack_pack_str_body(packer, bg_string_data(field_key), bg_string_length(field_key));

  /* value */
  msgpack_pack_bin(packer, bg_string_length(field_value));
  msgpack_pack_bin_body(packer, bg_string_data(field_value), bg_string_length(field_value));

  return 0;
}

int bg_persistence_msgpack_serialize_password(msgpack_packer *packer, bg_password* password) {
  bg_map *map = NULL;
  map = bg_password_to_map(password);
  if(!map) {return -1;}

  msgpack_pack_map(packer, bg_map_length(map));

  int err;
  if((err = bg_map_foreach(map,
                           (int (*)(const bg_string*, void*, void*))&serialize_field,
                           packer))) {
    return err;
  }

  return 0;
}

int bg_persistence_msgpack_deserialize_password(msgpack_object* object, bg_password* password) {
  msgpack_object_kv* keyvalue_iterator = object->via.map.ptr;
  int error_value = 0;
  char *name_iterator, *description_iterator;
  char *value_iterator;
  size_t name_size, description_size, value_size;

  if((error_value = get_keyvalue_iterator("name", &keyvalue_iterator, &name_iterator, &name_size,
                                          -3))) { return error_value; }
  ++keyvalue_iterator;
  if((error_value = get_keyvalue_iterator("description", &keyvalue_iterator, &description_iterator,
                                          &description_size, -4))) { return error_value; }
  ++keyvalue_iterator;
  if((error_value = get_keyvalue_iterator("value", &keyvalue_iterator, (char **)&value_iterator, &value_size,
                                          -5))) { return error_value; }

  if((error_value = bg_password_fill_raw(password, value_iterator, value_size))) {
    return error_value;
  }

  bg_string *name_buffer = bg_string_from_char_array(name_iterator, name_size);
  bg_password_update_name(password, name_buffer);

  bg_string *desc_buffer = bg_string_from_char_array(description_iterator, description_size);
  bg_password_update_description(password, desc_buffer);

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

int bg_persistence_msgpack_serialize_password_array(bg_msgpack_persister* self, msgpack_sbuffer* buffer, bg_repository_t *repo) {
  msgpack_packer pk;
  msgpack_packer_init(&pk, buffer, msgpack_sbuffer_write);

  struct serialize_data data = {
    .packer = &pk,
    .persister = self,
  };

  msgpack_pack_array(&pk, bg_repository_count(repo));
  return bg_repository_foreach(repo, &serialize_password, &data);
}

int bg_persistence_msgpack_deserialize_password_array(bg_msgpack_persister* self, unsigned char* data, size_t data_length, bg_repository_t *repo) {
  msgpack_zone mempool;
  msgpack_zone_init(&mempool, 2048);
  msgpack_object deserialized;
  msgpack_unpack((char*)data, data_length, NULL, &mempool, &deserialized);

  int i, err;
  msgpack_object* ptr = deserialized.via.array.ptr;
  for(i = 0; i < deserialized.via.array.size; ++i) {
    bg_password* password = bg_password_new();

    err = bg_persistence_msgpack_deserialize_password(ptr + i, password);
    if(err) { return err; }

    err = bg_repository_add(repo, password);
    if(err) { return err; }
  }

  msgpack_zone_destroy(&mempool);
  return 0;
}
