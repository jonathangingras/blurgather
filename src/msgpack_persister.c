#include <blurgather/msgpack_persister.h>
#include "msgpack_serialize.h"

static void bg_msgpack_persister_destroy(bg_persister_t *_self);
static int bg_msgpack_persister_load(bg_persister_t * self, bg_repository_t *repo);
static int bg_msgpack_persister_persist(bg_persister_t * self, bg_repository_t *repo);

static struct bg_persister_vtable bg_msgpack_persister_vtable = {
  .destroy = &bg_msgpack_persister_destroy,
  .load    = &bg_msgpack_persister_load,
  .persist = &bg_msgpack_persister_persist,
};

bg_msgpack_persister *bg_msgpack_persister_new(bg_string *filename, bg_cryptor_t *cryptor) {
  bg_msgpack_persister *self = malloc(sizeof(bg_msgpack_persister));

  self->persister.object = (void *) self;
  self->persister.vtable = &bg_msgpack_persister_vtable;

  self->persistence_filename = filename;

  self->cryptor = cryptor;
  self->secret_key = NULL;

  return self;
}

bg_persister_t *bg_msgpack_persister_persister(bg_msgpack_persister *persister) {
  return &persister->persister;
}

void bg_msgpack_persister_destroy(bg_persister_t *_self) {
  bg_msgpack_persister* self = (bg_msgpack_persister*) _self->object;

  bg_string_free(self->persistence_filename);
}

int bg_msgpack_persister_register_key(bg_persister_t *_self, bg_secret_key_t *secret_key) {
  bg_msgpack_persister* self = (bg_msgpack_persister*) _self->object;
  self->secret_key = secret_key;
  return 0;
}

int bg_msgpack_persister_unregister_key(bg_persister_t *_self) {
  bg_msgpack_persister* self = (bg_msgpack_persister*) _self->object;
  self->secret_key = NULL;
  return 0;
}

FILE *fmemopen(void *buf, size_t size, const char *mode);

int bg_msgpack_persister_persist(bg_persister_t * _self, bg_repository_t *repo) {
  int err = 0;
  bg_msgpack_persister* self = (bg_msgpack_persister*) _self->object;

  msgpack_sbuffer buffer;
  msgpack_sbuffer_init(&buffer);
  if((err = bg_persistence_msgpack_serialize_password_array(self, &buffer, repo))) {
    msgpack_sbuffer_destroy(&buffer);
    return err;
  }

  bg_iv_t *iv = NULL;
  size_t buffer_size = buffer.size;
  void *memory_buffer = malloc(buffer_size);

  if(!memory_buffer) {
    return -1;
  }
  memcpy(memory_buffer, buffer.data, buffer.size);
  msgpack_sbuffer_destroy(&buffer);

  FILE* shadow_file = fopen(bg_string_data(self->persistence_filename), "wb");

  if(self->secret_key && self->cryptor) {
    bg_cryptor_generate_iv(self->cryptor, &iv);
    bg_cryptor_encrypt(self->cryptor, memory_buffer, buffer_size, self->secret_key, iv);
    fwrite(bg_iv_data(iv), 1, bg_iv_length(iv), shadow_file);
  }

  fwrite(memory_buffer, 1, buffer_size, shadow_file);
  fclose(shadow_file);

  return 0;
}

int bg_msgpack_persister_load(bg_persister_t * _self, bg_repository_t *repo) {
  bg_msgpack_persister* self = (bg_msgpack_persister*) _self->object;

  FILE* shadow_file = fopen(bg_string_data(self->persistence_filename), "rb");
  if(!shadow_file) return -4;

  fseek(shadow_file, 0, SEEK_END);
  size_t data_length = ftell(shadow_file);
  fseek(shadow_file, 0, SEEK_SET);

  unsigned char* data = malloc(data_length);
  if(!data) { return -3; }

  if(fread(data, sizeof(char), data_length, shadow_file) != data_length) { return -2; }
  fclose(shadow_file);

  size_t data_offset = 0;
  if(self->secret_key && self->cryptor) {
    data_offset = bg_cryptor_iv_length(self->cryptor);
    bg_iv_t *iv = bg_iv_new(data, data_offset);
    bg_cryptor_decrypt(self->cryptor, data + data_offset, data_length - data_offset, self->secret_key, iv);
    bg_iv_free(iv);
  }

  int error_value = bg_persistence_msgpack_deserialize_password_array(self, data + data_offset, data_length - data_offset, repo);

  free(data);
  return error_value;
}
