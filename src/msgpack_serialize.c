#include "msgpack_serialize.h"

static int get_keyvalue_iterator(const char* field, msgpack_object_kv** keyvalue_iterator_ptr,
                          const char** iterator, size_t* size, int error_value) {
	if(memcmp(field, (*keyvalue_iterator_ptr)->key.via.str.ptr, (*keyvalue_iterator_ptr)->key.via.str.size)) {
		return error_value;
	}
	*iterator = (*keyvalue_iterator_ptr)->val.via.str.ptr;
	*size = (*keyvalue_iterator_ptr)->val.via.str.size;

	return 0;
}

void kiki_pwd_mng_Persistance_msgpack_serialize_kiPassword(msgpack_packer* packer, kiPassword* password) {
	msgpack_pack_map(packer, 5);

	msgpack_pack_str(packer, 4);
	msgpack_pack_str_body(packer, "uuid", 4);
	msgpack_pack_bin(packer, 16);
	msgpack_pack_bin_body(packer, password->uuid, 16);

	msgpack_pack_str(packer, 2);
	msgpack_pack_str_body(packer, "iv", 2);
	msgpack_pack_bin(packer, password->iv.length);
	msgpack_pack_bin_body(packer, password->iv.value, password->iv.length);

	msgpack_pack_str(packer, 4);
	msgpack_pack_str_body(packer, "name", 4);
	size_t name_len = strlen(password->name);
	msgpack_pack_str(packer, name_len);
	msgpack_pack_str_body(packer, password->name, name_len);

	msgpack_pack_str(packer, 11);
	msgpack_pack_str_body(packer, "description", 11);
	size_t description_len = strlen(password->description);
	msgpack_pack_str(packer, description_len);
	msgpack_pack_str_body(packer, password->description, description_len);

	msgpack_pack_str(packer, 5);
	msgpack_pack_str_body(packer, "value", 5);
	size_t value_len = kiki_reverse_memlen((unsigned char*)password->value, KIKI_PWD_MAX_VALUE_LEN);
	msgpack_pack_bin(packer, value_len);
	msgpack_pack_bin_body(packer, password->value, value_len);
}

int kiki_pwd_mng_Persistance_msgpack_deserialize_kiPassword(msgpack_object* object, kiPassword* password) {
	msgpack_object_kv* keyvalue_iterator = object->via.map.ptr;
	int error_value = 0;
	const char* uuid_iterator, * iv_iterator, * name_iterator, * description_iterator, * value_iterator;
	size_t uuid_size, iv_size, name_size, description_size, value_size;

	if((error_value = get_keyvalue_iterator("uuid", &keyvalue_iterator, &uuid_iterator, &uuid_size,
	                                        -1))) { return error_value; }
	++keyvalue_iterator;
	if((error_value = get_keyvalue_iterator("iv", &keyvalue_iterator, &iv_iterator, &iv_size,
	                                        -2))) { return error_value; }
	++keyvalue_iterator;
	if((error_value = get_keyvalue_iterator("name", &keyvalue_iterator, &name_iterator, &name_size,
	                                        -3))) { return error_value; }
	++keyvalue_iterator;
	if((error_value = get_keyvalue_iterator("description", &keyvalue_iterator, &description_iterator,
	                                        &description_size, -4))) { return error_value; }
	++keyvalue_iterator;
	if((error_value = get_keyvalue_iterator("value", &keyvalue_iterator, &value_iterator, &value_size,
	                                        -5))) { return error_value; }

	memcpy(password->uuid, uuid_iterator, uuid_size);
	memcpy(password->iv.value, iv_iterator, iv_size);
	password->iv.length = iv_size;
	memcpy(password->name, name_iterator, name_size);
	memcpy(password->description, description_iterator, description_size);
	memcpy(password->value, value_iterator, value_size);
	password->value_length = kiki_reverse_memlen((unsigned char*)password->value, KIKI_PWD_MAX_VALUE_LEN);
	password->crypted = 1;

	return error_value;
}

void kiki_pwd_mng_Persistance_msgpack_serialize_kiPasswordArray(kiPasswordMsgPackPersister* self, msgpack_sbuffer* buffer) {
	msgpack_packer pk;
	msgpack_packer_init(&pk, buffer, msgpack_sbuffer_write);

	msgpack_pack_array(&pk, self->number_passwords);
	int i;
	for(i = 0; i < self->number_passwords; ++i) {
		kiki_pwd_mng_Persistance_msgpack_serialize_kiPassword(&pk, self->password_array[i]);
	}
}

int kiki_pwd_mng_Persistance_msgpack_deserialize_kiPasswordArray(kiPasswordMsgPackPersister* self, kiPasswordFactory* factory, unsigned char* data, size_t data_length) {
	msgpack_zone mempool;
	msgpack_zone_init(&mempool, 2048);
	msgpack_object deserialized;
	msgpack_unpack((char*)data, data_length, NULL, &mempool, &deserialized);

	int i, error_value;
	msgpack_object* ptr = deserialized.via.array.ptr;
	for(i = 0; i < deserialized.via.array.size; ++i) {
		kiPassword* password = factory->new_kiPassword(factory);
		error_value = kiki_pwd_mng_Persistance_msgpack_deserialize_kiPassword(ptr + i, password);
		if(error_value) { return error_value; }
		error_value = password->save(password);
		if(error_value) { return error_value; }
	}

	msgpack_zone_destroy(&mempool);
	return 0;
}