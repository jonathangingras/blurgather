#include <string.h>
#include <blurgather/string.h>
#include "blurgather/secret_key.h"
#include "blurgather/password.h"

struct bg_secret_key {
	bg_string *str;
};

bg_secret_key_t* bg_secret_key_new(const void *value, size_t length) {
	bg_secret_key_t *self = malloc(sizeof(bg_secret_key_t));
	self->str = bg_string_from_char_array((char *)value, length);
	return self;
}

void bg_secret_key_free(bg_secret_key_t *self) {
	bg_secret_key_destroy(self);
	free(self);
}

//methods

void bg_secret_key_destroy(bg_secret_key_t *self) {
	bg_string_free(self->str);
}

int bg_secret_key_update(bg_secret_key_t *self, const void *memory, size_t length) {
	bg_string_clean(self->str);
	bg_string_free(self->str);
	self->str = bg_string_from_char_array((char *)memory, length);
	return self->str == NULL;
}

const void *bg_secret_key_data(bg_secret_key_t *self) {
	return bg_string_data(self->str);
}

size_t bg_secret_key_length(bg_secret_key_t *self) {
	return bg_string_length(self->str);
}