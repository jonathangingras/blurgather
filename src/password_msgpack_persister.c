#include "blurgather/password_msgpack_persister.h"
#include "msgpack_serialize.h"

static void bg_password_msgpack_persister_destroy(bg_password_repository* _self);
static int bg_password_msgpack_persister_add(bg_password_repository* self, bg_password* password);
static int bg_password_msgpack_persister_remove(bg_password_repository* self, const char *name);
static int bg_password_msgpack_persister_load(bg_password_repository* self);
static int bg_password_msgpack_persister_persist(bg_password_repository* self);
static void bg_password_msgpack_persister_sort(bg_password_repository* _self);

static bg_password_iterator bg_password_msgpack_persister_begin(bg_password_repository* _self);
static bg_password_iterator bg_password_msgpack_persister_end(bg_password_repository* _self);

static bg_password** bg_password_msgpack_persister_password_iterator_previous(bg_password_iterator* _self);
static bg_password** bg_password_msgpack_persister_password_iterator_next(bg_password_iterator* _self);

static struct bg_password_repository_vtable bg_password_msgpack_persister_vtable = {
  .destroy = &bg_password_msgpack_persister_destroy,
  .add = &bg_password_msgpack_persister_add,
  .remove = &bg_password_msgpack_persister_remove,
  .load = &bg_password_msgpack_persister_load,
  .persist = &bg_password_msgpack_persister_persist,
  .sort = &bg_password_msgpack_persister_sort,
  .begin = &bg_password_msgpack_persister_begin,
  .end = &bg_password_msgpack_persister_end
};

bg_password_msgpack_persister* bg_password_msgpack_persister_init(bg_password_msgpack_persister* _self, const char* filename, bg_password_factory* password_factory) {
	bg_password_msgpack_persister* self = _self ? _self : (bg_password_msgpack_persister*) malloc(sizeof(bg_password_msgpack_persister));
	
        self->repository.object = (void *) self;
	self->repository.vtable = &bg_password_msgpack_persister_vtable;

	self->password_factory = password_factory;
	self->number_passwords = 0;
	self->password_array = (bg_password_array) calloc(25, sizeof(bg_password*));
	self->allocated_length = 25;
	self->persistence_filename = calloc(strlen(filename), sizeof(char));
	strcat(self->persistence_filename, filename);

	return self;
}

static void bg_password_msgpack_persisterIterator_init(bg_password_msgpack_persister* self, bg_password_iterator* iterator) {
	iterator->container = self;
	iterator->previous = &bg_password_msgpack_persister_password_iterator_previous;
	iterator->next = &bg_password_msgpack_persister_password_iterator_next;
}

bg_password_iterator bg_password_msgpack_persister_begin(bg_password_repository* _self) {
	bg_password_msgpack_persister* self = (bg_password_msgpack_persister*) _self->object;

	bg_password_iterator iterator;
	bg_password_msgpack_persisterIterator_init(self, &iterator);
	iterator.value = &self->password_array[0];

	return iterator;
}

bg_password_iterator bg_password_msgpack_persister_end(bg_password_repository* _self) {
	bg_password_msgpack_persister* self = (bg_password_msgpack_persister*) _self->object;

	bg_password_iterator iterator;
	bg_password_msgpack_persisterIterator_init(self, &iterator);
	iterator.value = &self->password_array[self->number_passwords];

	return iterator;
}

bg_password** bg_password_msgpack_persister_password_iterator_previous(bg_password_iterator* self) {
	return --self->value;
}

bg_password** bg_password_msgpack_persister_password_iterator_next(bg_password_iterator* self) {
	return ++self->value;
}

void bg_password_msgpack_persister_free(bg_password_msgpack_persister* self) {
	bg_pwd_repository_destroy(&self->repository);
	free(self);
}

//methods

void bg_password_msgpack_persister_destroy(bg_password_repository* _self) {
  bg_password_msgpack_persister* self = (bg_password_msgpack_persister*) _self->object;

  free(self->persistence_filename);
  int i;
  for(i = 0; i < self->number_passwords; ++i) {
    bg_password_free(self->password_array[i]);
  }
  free(self->password_array);
}

static bg_password* find_password_by_name(bg_password_msgpack_persister* self, const char *name, int* index_found) {
	bg_password* password = NULL;
	int i;
  char name_[BLURGATHER_PWD_MAX_NAME_LEN];
  memset(name_, 0, BLURGATHER_PWD_MAX_NAME_LEN);
  strcat(name_, name);

	for(i = 0; i < self->number_passwords; ++i) {
		if(memcmp(bg_password_name(self->password_array[i]), name_, BLURGATHER_PWD_MAX_NAME_LEN) == 0) {
			password = self->password_array[i];
			if(index_found) {
				*index_found = i;
			}
		}
	}

	return password;
}

static int add_new_password(bg_password_msgpack_persister* self, bg_password* password) {
	if(self->allocated_length <= self->number_passwords + 1) {
		self->password_array = (bg_password_array) realloc(self->password_array, self->allocated_length + 25);
		self->allocated_length += 25;
	}

	self->password_array[self->number_passwords] = password;
	self->number_passwords++;
	return 0;
}

int bg_password_msgpack_persister_add(bg_password_repository* _self, bg_password* _password) {
	bg_password_msgpack_persister* self = (bg_password_msgpack_persister*) _self->object;

	bg_password* password = find_password_by_name(self, bg_password_name(_password), NULL);

	if(!password) {
		return add_new_password(self, _password);
	}
	return -1;
}

int bg_password_msgpack_persister_remove(bg_password_repository* _self, const char *name) {
	bg_password_msgpack_persister* self = (bg_password_msgpack_persister*) _self->object;

	bg_password* password;
	int password_index;
	if((password = find_password_by_name(self, name, &password_index))) {
		self->password_array[password_index] = NULL;

		if(password_index != self->number_passwords - 1 && self->number_passwords > 1) {
			self->password_array[password_index] = self->password_array[self->number_passwords - 1];
			self->password_array[self->number_passwords - 1] = NULL;
		}

		self->number_passwords--;
		bg_password_free(password);
		return 0;
	}
	return -1;
}

int bg_password_msgpack_persister_persist(bg_password_repository* _self) {
	bg_password_msgpack_persister* self = (bg_password_msgpack_persister*) _self->object;

	msgpack_sbuffer buffer;
	msgpack_sbuffer_init(&buffer);
	bg_persistence_msgpack_serialize_password_array(self, &buffer);

	FILE* shadow_file = fopen(self->persistence_filename, "wb");
	fwrite(&buffer.size, sizeof(size_t), 1, shadow_file);
	fwrite(buffer.data, sizeof(unsigned char), buffer.size, shadow_file);
	fclose(shadow_file);

	msgpack_sbuffer_destroy(&buffer);
	return 0;
}

int bg_password_msgpack_persister_load(bg_password_repository* _self) {
	bg_password_msgpack_persister* self = (bg_password_msgpack_persister*) _self->object;

	FILE* shadow_file = fopen(self->persistence_filename, "rb");
	if(!shadow_file) return -4;

	size_t data_length;
	if(fread(&data_length, sizeof(size_t), 1, shadow_file) != 1) { return -1; }
	unsigned char* data = (unsigned char*) malloc(data_length * (sizeof(unsigned char)));
	if(!data) { return -3; }

	if(fread(data, sizeof(char), data_length, shadow_file) != data_length) { return -2; }
	fclose(shadow_file);

	int error_value = bg_persistence_msgpack_deserialize_password_array(self, self->password_factory, data,
	                                                                               data_length);

	free(data);
	return error_value;
}

static int compare_passwords(const void* _pass1, const void* _pass2) {
	bg_password* pass1 = *(bg_password**) _pass1, * pass2 = *(bg_password**) _pass2;
	return strcmp(bg_password_name(pass1), bg_password_name(pass2));
}

void bg_password_msgpack_persister_sort(bg_password_repository* _self) {
	bg_password_msgpack_persister* self = (bg_password_msgpack_persister*) _self->object;

	if(self->number_passwords < 2) return;

	qsort(self->password_array, self->number_passwords, sizeof(bg_password*), &compare_passwords);
}
