#include <blurgather/msgpack_persister.h>
#include "msgpack_serialize.h"

static void bg_msgpack_persister_destroy(bg_persister_t *_self);
static int bg_msgpack_persister_load(bg_persister_t * self);
static int bg_msgpack_persister_persist(bg_persister_t * self);

static struct bg_persister_vtable bg_msgpack_persister_vtable = {
  .destroy = &bg_msgpack_persister_destroy,
  .load    = &bg_msgpack_persister_load,
  .persist = &bg_msgpack_persister_persist,
};

bg_msgpack_persister *bg_msgpack_persister_new(bg_context *ctx, const bg_string *filename) {
  bg_msgpack_persister *self = bgctx_allocate(ctx, sizeof(bg_msgpack_persister));

  self->ctx = ctx;
  self->persister.object = (void *) self;
  self->persister.vtable = &bg_msgpack_persister_vtable;

  self->persistence_filename = bg_string_copy(filename);

  return self;
}

bg_persister_t *bg_msgpack_persister_persister(bg_msgpack_persister *persister) {
  return &persister->persister;
}

void bg_msgpack_persister_destroy(bg_persister_t *_self) {
  bg_msgpack_persister* self = (bg_msgpack_persister*) _self->object;

  bg_string_free(self->persistence_filename);
}

void bg_msgpack_persister_free(bg_msgpack_persister *self) {
  bg_msgpack_persister_destroy(&self->persister);
  bgctx_deallocate(self->ctx, self);
}

int bg_msgpack_persister_persist(bg_persister_t * _self) {
	bg_msgpack_persister* self = (bg_msgpack_persister*) _self->object;

	msgpack_sbuffer buffer;
	msgpack_sbuffer_init(&buffer);
	bg_persistence_msgpack_serialize_password_array(self, &buffer);

	FILE* shadow_file = fopen(bg_string_data(self->persistence_filename), "wb");
	fwrite(&buffer.size, sizeof(size_t), 1, shadow_file);
	fwrite(buffer.data, sizeof(unsigned char), buffer.size, shadow_file);
	fclose(shadow_file);

	msgpack_sbuffer_destroy(&buffer);
	return 0;
}

int bg_msgpack_persister_load(bg_persister_t * _self) {
	bg_msgpack_persister* self = (bg_msgpack_persister*) _self->object;

	FILE* shadow_file = fopen(bg_string_data(self->persistence_filename), "rb");
	if(!shadow_file) return -4;

	size_t data_length;
	if(fread(&data_length, sizeof(size_t), 1, shadow_file) != 1) { return -1; }
	unsigned char* data = (unsigned char*) bgctx_allocate(self->ctx, data_length * (sizeof(unsigned char)));
	if(!data) { return -3; }

	if(fread(data, sizeof(char), data_length, shadow_file) != data_length) { return -2; }
	fclose(shadow_file);

	int error_value = bg_persistence_msgpack_deserialize_password_array(self, data, data_length);

	free(data);
	return error_value;
}
