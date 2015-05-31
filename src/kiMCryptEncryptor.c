#include <stdio.h>
#include <string.h>
#include <kiki/utilities.h>

#include "kiMCryptEncryptor.h"

static void kiki_pwd_mng_kiMCryptEncryptor_destroy(kiMCryptEncryptor* self);
static int kiki_pwd_mng_kiMCryptEncryptor_crypt(kiEncryptor* _self, void* memory,
                                                        size_t* input_length);
static int kiki_pwd_mng_kiMCryptEncryptor_decrypt(kiDecryptor* _self, void* memory,
                                                          size_t* input_length);
static void kiki_pwd_mng_kiMCryptEncryptor_set_iv(kiEncryptor* _self, IV_t);
static int kiki_pwd_mng_kiMCryptEncryptor_set_secret_key(kiEncryptor* _self, kiki_pwd_mng_secret_key_t*);

kiMCryptEncryptor* kiki_pwd_mng_kiMCryptEncryptor_init(kiMCryptEncryptor* _self) {
	kiMCryptEncryptor* self = _self;
	if(!_self) { self = (kiMCryptEncryptor*) malloc(sizeof(kiMCryptEncryptor)); }

	self->secret_key = kiki_pwd_mng_secret_key_t_init(NULL);
	self->iv = (IV_t)calloc(16, sizeof(unsigned char));

	self->destroy = &kiki_pwd_mng_kiMCryptEncryptor_destroy;

	self->encryptor.object = (void*) self;
	self->encryptor.crypt = &kiki_pwd_mng_kiMCryptEncryptor_crypt;
	self->encryptor.set_iv = &kiki_pwd_mng_kiMCryptEncryptor_set_iv;
	self->encryptor.set_secret_key = &kiki_pwd_mng_kiMCryptEncryptor_set_secret_key;

	self->decryptor.object = (void*) self;
	self->decryptor.decrypt = &kiki_pwd_mng_kiMCryptEncryptor_decrypt;
	self->decryptor.set_iv = (void (*)(kiDecryptor*,IV_t))&kiki_pwd_mng_kiMCryptEncryptor_set_iv;
	self->decryptor.set_secret_key = (int (*)(kiDecryptor*,kiki_pwd_mng_secret_key_t*))&kiki_pwd_mng_kiMCryptEncryptor_set_secret_key;

	return self;
}

void kiki_pwd_mng_kiMCryptEncryptor_free(kiMCryptEncryptor* self) {
	self->destroy(self);
	free(self);
}

//methods

void kiki_pwd_mng_kiMCryptEncryptor_destroy(kiMCryptEncryptor* self) {
	self->secret_key->destroy(self->secret_key);
	free(self->iv);
}

int kiki_pwd_mng_kiMCryptEncryptor_crypt(kiEncryptor* _self, void* memory,
                                                 size_t* input_length) {
	kiMCryptEncryptor* self = (kiMCryptEncryptor*) _self->object;
	if(!self->secret_key->length) { return -1; }
	if(!self->iv[0]) { return -2; }

	MCRYPT mcrypt_thread_descriptor = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
	int block_size = mcrypt_enc_get_block_size(mcrypt_thread_descriptor);
	while(*input_length % block_size) { ++*input_length; }

	mcrypt_generic_init(mcrypt_thread_descriptor, self->secret_key->value, self->secret_key->length, self->iv);
	mcrypt_generic(mcrypt_thread_descriptor, memory, *input_length);
	mcrypt_generic_deinit(mcrypt_thread_descriptor);
	mcrypt_module_close(mcrypt_thread_descriptor);

	return 0;
}

int kiki_pwd_mng_kiMCryptEncryptor_decrypt(kiDecryptor* _self, void* memory,
                                                   size_t* input_length) {
	kiMCryptEncryptor* self = (kiMCryptEncryptor*) _self->object;
	if(!self->secret_key->length) return -1;
	if(!self->iv[0]) return -2;

	MCRYPT mcrypt_thread_descriptor = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
	int block_size = mcrypt_enc_get_block_size(mcrypt_thread_descriptor);
	if(*input_length % block_size != 0) { return -3; }

	mcrypt_generic_init(mcrypt_thread_descriptor, self->secret_key->value, self->secret_key->length, self->iv);
	mdecrypt_generic(mcrypt_thread_descriptor, memory, *input_length);
	mcrypt_generic_deinit(mcrypt_thread_descriptor);
	mcrypt_module_close(mcrypt_thread_descriptor);

	*input_length = kiki_reverse_memlen(memory, *input_length);

	return 0;
}

void kiki_pwd_mng_kiMCryptEncryptor_set_iv(kiEncryptor* _self, IV_t iv) {
	kiMCryptEncryptor* self = (kiMCryptEncryptor*) _self->object;
	memcpy(self->iv, iv, 16);
}

int kiki_pwd_mng_kiMCryptEncryptor_set_secret_key(kiEncryptor* _self,
                                                          kiki_pwd_mng_secret_key_t* secret_key) {
	kiMCryptEncryptor* self = (kiMCryptEncryptor*) _self->object;

	if(self->secret_key == secret_key) { return -1; }

	self->secret_key->destroy(self->secret_key);
	self->secret_key = secret_key;
	return 0;
}