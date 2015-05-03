#include <kiki/pwd_mng/kiPasswordMsgPackPersister.h>
#include "msgpack_serialize.h"

static void kiki_pwd_mng_kiPasswordMsgPackPersister_destroy(kiPasswordMsgPackPersister* self);
static int kiki_pwd_mng_kiPasswordMsgPackPersister_add(kiPasswordRepository* self, kiPassword* password);
static int kiki_pwd_mng_kiPasswordMsgPackPersister_remove(kiPasswordRepository* self, uuid_t password_uuid);
static int kiki_pwd_mng_kiPasswordMsgPackPersister_load(kiPasswordRepository* self);
static int kiki_pwd_mng_kiPasswordMsgPackPersister_persist(kiPasswordRepository* self);
static void kiki_pwd_mng_kiPasswordMsgPackPersister_sort(kiPasswordRepository* _self);

kiPasswordMsgPackPersister* kiki_pwd_mng_kiPasswordMsgPackPersister_init(kiPasswordMsgPackPersister* _self,
                                                                         const char* fileName,
                                                                         kiPasswordFactory* password_factory) {
	kiPasswordMsgPackPersister* self = _self ? _self : (kiPasswordMsgPackPersister*) malloc(
		sizeof(kiPasswordMsgPackPersister));

	self->destroy = &kiki_pwd_mng_kiPasswordMsgPackPersister_destroy;
	self->repository.object = (void*) self;
	self->repository.add = &kiki_pwd_mng_kiPasswordMsgPackPersister_add;
	self->repository.remove = &kiki_pwd_mng_kiPasswordMsgPackPersister_remove;
	self->repository.load = &kiki_pwd_mng_kiPasswordMsgPackPersister_load;
	self->repository.persist = &kiki_pwd_mng_kiPasswordMsgPackPersister_persist;
	self->repository.sort = &kiki_pwd_mng_kiPasswordMsgPackPersister_sort;

	self->password_factory = password_factory;
	self->number_passwords = 0;
	self->password_array = (kiPasswordArray) calloc(25, sizeof(kiPassword*));
	self->allocated_length = 25;
	self->persistanceFileName = calloc(strlen(fileName), sizeof(char));
	strcat(self->persistanceFileName, fileName);

	return self;
}

void kiki_pwd_mng_kiPasswordMsgPackPersister_free(kiPasswordMsgPackPersister* self) {
	self->destroy(self);
	free(self);
}

//methods

void kiki_pwd_mng_kiPasswordMsgPackPersister_destroy(kiPasswordMsgPackPersister* self) {
	free(self->persistanceFileName);
	int i;
	for(i = 0; i < self->number_passwords; ++i) {
		kiki_pwd_mng_kiPassword_free(self->password_array[i]);
	}
	free(self->password_array);
}

static kiPassword* find_password_by_uuid(kiPasswordMsgPackPersister* self, uuid_t uuid, int* index_found) {
	kiPassword* password = NULL;
	int i;
	for(i = 0; i < self->number_passwords; ++i) {
		if(memcmp(self->password_array[i]->uuid, uuid, 16) == 0) {
			password = self->password_array[i];
			if(index_found) {
				*index_found = i;
			}
		}
	}

	return password;
}

static int add_new_password(kiPasswordMsgPackPersister* self, kiPassword* password) {
	if(self->allocated_length <= self->number_passwords + 1) {
		self->password_array = (kiPasswordArray) realloc(self->password_array, self->allocated_length + 25);
		self->allocated_length += 25;
	}

	self->password_array[self->number_passwords] = password;
	self->number_passwords++;
	return 0;
}

int kiki_pwd_mng_kiPasswordMsgPackPersister_add(kiPasswordRepository* _self, kiPassword* _password) {
	kiPasswordMsgPackPersister* self = (kiPasswordMsgPackPersister*) _self->object;

	kiPassword* password = find_password_by_uuid(self, _password->uuid, NULL);

	if(!password) {
		return add_new_password(self, _password);
	}
	return 0;
}

int kiki_pwd_mng_kiPasswordMsgPackPersister_remove(kiPasswordRepository* _self, uuid_t password_uuid) {
	kiPasswordMsgPackPersister* self = (kiPasswordMsgPackPersister*) _self->object;

	kiPassword* password;
	int password_index;
	if((password = find_password_by_uuid(self, password_uuid, &password_index))) {
		self->password_array[password_index] = NULL;

		if(password_index != self->number_passwords - 1 && self->number_passwords > 1) {
			self->password_array[password_index] = self->password_array[self->number_passwords - 1];
			self->password_array[self->number_passwords - 1] = NULL;
		}

		self->number_passwords--;
		kiki_pwd_mng_kiPassword_free(password);
		return 0;
	}
	return -1;
}

int kiki_pwd_mng_kiPasswordMsgPackPersister_persist(kiPasswordRepository* _self) {
	kiPasswordMsgPackPersister* self = (kiPasswordMsgPackPersister*) _self->object;

	msgpack_sbuffer buffer;
	msgpack_sbuffer_init(&buffer);
	kiki_pwd_mng_Persistance_msgpack_serialize_kiPasswordArray(self, &buffer);

	FILE* shadow_file = fopen(self->persistanceFileName, "wb");
	fwrite(&buffer.size, sizeof(size_t), 1, shadow_file);
	fwrite(buffer.data, sizeof(unsigned char), buffer.size, shadow_file);
	fclose(shadow_file);

	msgpack_sbuffer_destroy(&buffer);
	return 0;
}

int kiki_pwd_mng_kiPasswordMsgPackPersister_load(kiPasswordRepository* _self) {
	kiPasswordMsgPackPersister* self = (kiPasswordMsgPackPersister*) _self->object;

	FILE* shadow_file = fopen(self->persistanceFileName, "rb");
	unsigned char* data = (unsigned char*) malloc(2555 * (sizeof(char)));
	size_t data_length;
	if(fread(&data_length, sizeof(size_t), 1, shadow_file) != 1) return -1;
	if(fread(data, sizeof(char), data_length, shadow_file) != data_length) return -2;
	fclose(shadow_file);

	int error_value = kiki_pwd_mng_Persistance_msgpack_deserialize_kiPasswordArray(self, self->password_factory, data,
	                                                                               data_length);

	free(data);
	return error_value;
}

static int compare_kiPasswords(const void* _pass1, const void* _pass2) {
	kiPassword* pass1 = *(kiPassword**) _pass1, * pass2 = *(kiPassword**) _pass2;
	return strcmp(pass1->name, pass2->name);
}

void kiki_pwd_mng_kiPasswordMsgPackPersister_sort(kiPasswordRepository* _self) {
	kiPasswordMsgPackPersister* self = (kiPasswordMsgPackPersister*) _self->object;

	if(self->number_passwords < 2) return;

	qsort(self->password_array, self->number_passwords, sizeof(kiPassword*), &compare_kiPasswords);
}