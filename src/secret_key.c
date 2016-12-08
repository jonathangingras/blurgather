#include <blurgather/string.h>
#include <blurgather/secret_key.h>
#include <blurgather/context.h>

struct bg_secret_key_t {
	bg_string *str;
};

bg_secret_key_t* bg_secret_key_new(bg_context *ctx, const void *value, size_t length) {
	bg_secret_key_t *self = bgctx_allocate(ctx, sizeof(bg_secret_key_t));
	self->str = bg_string_from_char_array((char *)value, length);
	return self;
}

void bg_secret_key_free(bg_secret_key_t *self) {
  bg_string_clean_free(self->str);
	free(self);
}

/* methods */

int bg_secret_key_update(bg_secret_key_t *self, const void *memory, size_t length) {
	bg_string_clean(self->str);
	bg_string_free(self->str);
	self->str = bg_string_from_char_array((char *)memory, length);
	return self->str == NULL;
}

const void *bg_secret_key_data(const bg_secret_key_t *self) {
	return bg_string_data(self->str);
}

size_t bg_secret_key_length(const bg_secret_key_t *self) {
	return bg_string_length(self->str);
}
