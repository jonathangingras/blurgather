#include <kiki/pwd_mng/kiPassword.h>

static void kiki_pwd_mng_secret_key_t_destroy(kiki_pwd_mng_secret_key_t* self);
static int kiki_pwd_mng_secret_key_t_update(kiki_pwd_mng_secret_key_t* self, const void* memory, size_t length);

kiki_pwd_mng_secret_key_t* kiki_pwd_mng_secret_key_t_init(kiki_pwd_mng_secret_key_t* _self) {
	kiki_pwd_mng_secret_key_t* self = _self ? _self : (kiki_pwd_mng_secret_key_t*)malloc(sizeof(kiki_pwd_mng_secret_key_t));
	self->value = (char*)calloc(KIKI_PWD_MAX_VALUE_LEN, sizeof(char));
	self->length = 0;

	self->destroy = &kiki_pwd_mng_secret_key_t_destroy;
	self->update = &kiki_pwd_mng_secret_key_t_update;
}

void kiki_pwd_mng_secret_key_t_free(kiki_pwd_mng_secret_key_t* self) {
	self->destroy(self);
	free(self);
}

//methods

void kiki_pwd_mng_secret_key_t_destroy(kiki_pwd_mng_secret_key_t* self) {
	free(self->value);
}

int kiki_pwd_mng_secret_key_t_update(kiki_pwd_mng_secret_key_t* self, const void* memory, size_t length) {
	memset(self->value, 0, KIKI_PWD_MAX_VALUE_LEN);
	memcpy(self->value, memory, length);
	self->length = length;
	return 0;
}