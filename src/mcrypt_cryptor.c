#include <stdio.h>

#include "utilities.h"
#include "mcrypt_cryptor.h"

static void bg_mcrypt_cryptor_destroy(bg_mcrypt_cryptor* self);
static int bg_mcrypt_cryptor_crypt(bg_encryptor* _self, void *memory, size_t memlen, size_t max_length, size_t *writelen);
static int bg_mcrypt_cryptor_decrypt(bg_decryptor* _self, void *memory, size_t memlen, size_t max_length, size_t *writelen);
static void bg_mcrypt_cryptor_set_iv(bg_encryptor* _self, IV_t*);
static int bg_mcrypt_cryptor_set_secret_key(bg_encryptor* _self, bg_secret_key*);

bg_mcrypt_cryptor* bg_mcrypt_cryptor_init(bg_mcrypt_cryptor* _self) {
	bg_mcrypt_cryptor* self = _self;
	if(!_self) { self = (bg_mcrypt_cryptor*) malloc(sizeof(bg_mcrypt_cryptor)); }

	self->secret_key = bg_secret_key_init(NULL);
	self->iv = NULL;

	self->destroy = &bg_mcrypt_cryptor_destroy;

	self->encryptor.object = (void*) self;
	self->encryptor.crypt = &bg_mcrypt_cryptor_crypt;
	self->encryptor.set_iv = &bg_mcrypt_cryptor_set_iv;
	self->encryptor.set_secret_key = &bg_mcrypt_cryptor_set_secret_key;

	self->decryptor.object = (void*) self;
	self->decryptor.decrypt = &bg_mcrypt_cryptor_decrypt;
	self->decryptor.set_iv = (void (*)(bg_decryptor*,IV_t*))&bg_mcrypt_cryptor_set_iv;
	self->decryptor.set_secret_key = (int (*)(bg_decryptor*,bg_secret_key*))&bg_mcrypt_cryptor_set_secret_key;

	return self;
}

void bg_mcrypt_cryptor_free(bg_mcrypt_cryptor* self) {
	self->destroy(self);
	free(self);
}

//methods

void bg_mcrypt_cryptor_destroy(bg_mcrypt_cryptor* self) {
	self->secret_key->destroy(self->secret_key);
}

int bg_mcrypt_cryptor_crypt(bg_encryptor* _self, void *memory, size_t memlen, size_t max_length, size_t *writelen) {
	bg_mcrypt_cryptor* self = (bg_mcrypt_cryptor*) _self->object;
	if(!self->secret_key->length) { return -1; }
	if(!self->iv) { return -2; }
        
        size_t input_length = memlen;

	MCRYPT mcrypt_thread_descriptor = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);

	int block_size = mcrypt_enc_get_block_size(mcrypt_thread_descriptor);
	while(input_length % block_size) { ++input_length; }
        if(writelen) { *writelen = input_length; }

        if(input_length > max_length) {
          mcrypt_module_close(mcrypt_thread_descriptor);
          return -3;
        }

	mcrypt_generic_init(mcrypt_thread_descriptor, self->secret_key->value, self->secret_key->length, self->iv->value);
	mcrypt_generic(mcrypt_thread_descriptor, memory, input_length);
	mcrypt_generic_deinit(mcrypt_thread_descriptor);
	mcrypt_module_close(mcrypt_thread_descriptor);

	return 0;
}

int bg_mcrypt_cryptor_decrypt(bg_decryptor* _self, void *memory, size_t memlen, size_t max_length, size_t *writelen) {
	bg_mcrypt_cryptor* self = (bg_mcrypt_cryptor*) _self->object;
	if(!self->secret_key->length) return -1;
	if(!self->iv) return -2;

        size_t input_length = memlen;

	MCRYPT mcrypt_thread_descriptor = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
	int block_size = mcrypt_enc_get_block_size(mcrypt_thread_descriptor);
	if(input_length % block_size != 0 || input_length > max_length) { 
          mcrypt_module_close(mcrypt_thread_descriptor);
          return -3;
        }

	mcrypt_generic_init(mcrypt_thread_descriptor, self->secret_key->value, self->secret_key->length, self->iv->value);
	mdecrypt_generic(mcrypt_thread_descriptor, memory, input_length);
	mcrypt_generic_deinit(mcrypt_thread_descriptor);
	mcrypt_module_close(mcrypt_thread_descriptor);

	if(writelen) { *writelen = bg_reverse_memlen(memory, input_length); }

	return 0;
}

void bg_mcrypt_cryptor_set_iv(bg_encryptor* _self, IV_t* iv) {
	bg_mcrypt_cryptor* self = (bg_mcrypt_cryptor*) _self->object;
	self->iv = iv;
}

int bg_mcrypt_cryptor_set_secret_key(bg_encryptor* _self,
                                                          bg_secret_key* secret_key) {
	bg_mcrypt_cryptor* self = (bg_mcrypt_cryptor*) _self->object;

	if(self->secret_key == secret_key) { return -1; }

	self->secret_key->destroy(self->secret_key);
	self->secret_key = secret_key;
	return 0;
}
