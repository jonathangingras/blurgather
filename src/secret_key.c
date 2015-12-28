#include "secret_key.h"
#include "password.h"

static void bg_secret_key_destroy(bg_secret_key* self);
static int bg_secret_key_update(bg_secret_key* self, const void* memory, size_t length);

bg_secret_key* bg_secret_key_init(bg_secret_key* _self) {
	bg_secret_key* self = _self ? _self : (bg_secret_key*)malloc(sizeof(bg_secret_key));
	self->value = (unsigned char*)calloc(BLURGATHER_PWD_MAX_VALUE_LEN, sizeof(char));
	self->length = 0;

	self->destroy = &bg_secret_key_destroy;
	self->update = &bg_secret_key_update;
        
        return self;
}

void bg_secret_key_free(bg_secret_key* self) {
	self->destroy(self);
	free(self);
}

//methods

void bg_secret_key_destroy(bg_secret_key* self) {
	free(self->value);
}

int bg_secret_key_update(bg_secret_key* self, const void* memory, size_t length) {
	memset(self->value, 0, BLURGATHER_PWD_MAX_VALUE_LEN);
	memcpy(self->value, memory, length);
	self->length = length;
	return 0;
}
