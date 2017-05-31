#include "mocks.h"

bg_context *ctx = NULL;
FILE *logstream = NULL;

int mock_encrypt_called = 0;
int mock_decrypt_called = 0;
int mock_encrypt_return_value = 0;
int mock_decrypt_return_value = 0;
int mock_cryptor_generate_iv_called = 0;
bg_iv_t *mock_iv = NULL;
char mock_iv_data[32] = "1111111111111111111111111111111";
size_t mock_repository_count_return_value = 0;


size_t allocated = 0;
size_t reallocated = 0;


void *mock_allocate(size_t size);
void mock_deallocate(void *object);
void *mock_reallocate(void *object, size_t size);

bg_allocator_t mock_allocator = {
  .allocate = &mock_allocate,
  .deallocate = &mock_deallocate,
  .reallocate = &mock_reallocate,
};

int mock_encrypt(void *memory,
                 size_t memlen,
                 const bg_secret_key_t *secret_key,
                 const bg_iv_t *iv);
int mock_decrypt(void *memory,
                 size_t memlen,
                 const bg_secret_key_t *secret_key,
                 const bg_iv_t *iv);
size_t mock_iv_length();
int mock_generate_iv(bg_iv_t **output);

bg_cryptor_t mock_cryptor = {
  .encrypt = &mock_encrypt,
  .decrypt = &mock_decrypt,
  .iv_length = &mock_iv_length,
  .generate_iv = &mock_generate_iv,
};

void mock_repository_destroy(bg_repository_t *self);
int mock_repository_add(bg_repository_t *self, bg_password *password);
int mock_repository_get(bg_repository_t *self, const bg_string *name, bg_password **password);
int mock_repository_remove(bg_repository_t *self, const bg_string *name);
size_t mock_repository_count(bg_repository_t *self);
int mock_repository_foreach(bg_repository_t *self, int (* callback)(bg_password *, void *), void *output);

struct bg_repository_vtable mock_repository_vtable = {
  .destroy = &mock_repository_destroy,
  .add = &mock_repository_add,
  .get = &mock_repository_get,
  .remove = &mock_repository_remove,
  .count = &mock_repository_count,
  .foreach = &mock_repository_foreach,
};

bg_repository_t mock_repository = {
  .object = &mock_repository,
  .vtable = &mock_repository_vtable,
};


void mock_persister_destroy(bg_persister_t *self);
int mock_persister_load(bg_persister_t *self);
int mock_persister_persist(bg_persister_t *self);

struct bg_persister_vtable mock_persister_vtable = {
  .destroy = &mock_persister_destroy,
  .load = &mock_persister_load,
  .persist = &mock_persister_persist,
};

bg_persister_t mock_persister = {
  .object = &mock_persister,
  .vtable = &mock_persister_vtable,
};

/* implementation */

/* helper test functions */

void reset_debug() {
  if(logstream != stdout && logstream != NULL) {
    fclose(logstream);
  }
  logstream = stdout;
}

void turnoff_debug() {
  logstream = fopen("/dev/null", "w");
}

void reset_context() {
  if(ctx) {
    free(ctx);
  }
  bgctx_init(&ctx);
  bgctx_unlock(ctx, bg_secret_key_new("secret", 6));
}

void reset_allocator() {
  allocated = 0;
  reallocated = 0;
}

void reset_mock_cryptor() {
  mock_encrypt_called = 0;
  mock_decrypt_called = 0;
  mock_encrypt_return_value = 0;
  mock_decrypt_return_value = 0;
  if(mock_iv) {
    bg_iv_free(mock_iv);
  }
  mock_iv = bg_iv_new(mock_iv_data, 32);
}

void reset_mock_repository() {
  mock_repository_destroy_called = 0;
  mock_repository_add_called = 0;
  mock_repository_get_called = 0;
  mock_repository_remove_called = 0;
  mock_repository_count_called = 0;
  mock_persister_load_called = 0;
  mock_persister_persist_called = 0;
}


/* mock/inspect memory allocation */

void *mock_allocate(size_t size) {
  fprintf(logstream, "[Memory ALLOCATION of %zu bytes requested] -->> ", size);
  fflush(logstream);

  void *mem = malloc(size);

  fprintf(logstream, "OK, new address: %p\n", mem);
  allocated += size;
  return mem;
}

void mock_deallocate(void *object) {
  fprintf(logstream, "[Memory DEALLOCATION requested, address: %p] -->> ", object);
  fflush(logstream);

  if(object == mock_iv) {
    fprintf(logstream, "was mock memory\n");
    return;
  }

  free(object);

  fprintf(logstream, "OK\n");
}

void *mock_reallocate(void *object, size_t size) {
  fprintf(logstream, "[Memory REALLOCATION of %zu bytes requested, old address: %p] -->> ", size, object);
  fflush(logstream);

  void *mem = realloc(object, size);

  fprintf(logstream, "OK, new address: %p\n", mem);
  reallocated += size;
  return mem;
}


/* cryptor */

int mock_encrypt(void *memory,
                 size_t memlen,
                 const bg_secret_key_t *secret_key,
                 const bg_iv_t *iv) {
  mock_encrypt_called = 1;

  return mock_encrypt_return_value;
}

int mock_decrypt(void *memory,
                 size_t memlen,
                 const bg_secret_key_t *secret_key,
                 const bg_iv_t *iv) {
  mock_decrypt_called = 1;

  return mock_decrypt_return_value;
}

size_t mock_iv_length() {
  return 32;
}

int mock_generate_iv(bg_iv_t **output) {
  mock_cryptor_generate_iv_called = 1;
  *output = bg_iv_new(mock_iv_data, 32);
  return 0;
}


/* repository */

int mock_repository_destroy_called = 0;
int mock_repository_add_called = 0;
int mock_repository_get_called = 0;
int mock_repository_remove_called = 0;
int mock_repository_count_called = 0;
int mock_repository_foreach_called = 0;

void mock_repository_destroy(bg_repository_t *self) {
  mock_repository_destroy_called += 1;
}

int mock_repository_add(bg_repository_t *self, bg_password *password) {
  mock_repository_add_called += 1;
  return 0;
}

int mock_repository_get(bg_repository_t *self, const bg_string *name, bg_password **password) {
  mock_repository_get_called += 1;
  return 0;
}

int mock_repository_remove(bg_repository_t *self, const bg_string *name) {
  mock_repository_remove_called += 1;
  return 0;
}

size_t mock_repository_count(bg_repository_t *self) {
  mock_repository_count_called += 1;
  return mock_repository_count_return_value;
}

int mock_repository_foreach(bg_repository_t *self, int (* callback)(bg_password *, void *), void *output) {
  mock_repository_foreach_called += 1;
  return 0;
}


/* persister */

int mock_persister_load_called = 0;
int mock_persister_persist_called = 0;

void mock_persister_destroy(bg_persister_t *self) {

}

int mock_persister_load(bg_persister_t *self) {
  mock_persister_load_called = 1;
  return 0;
}

int mock_persister_persist(bg_persister_t *self) {
  mock_persister_persist_called = 1;
  return 0;
}
