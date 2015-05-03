#include <kiki/pwd_mng/kiPasswordMCryptEncryptor.h>

static void kiki_pwd_mng_kiPasswordMCryptEncryptor_destroy(kiPasswordMCryptEncryptor* self);
static int kiki_pwd_mng_kiPasswordMCryptEncryptor_crypt(kiPasswordEncryptor* _self, void* memory,
                                                        size_t input_length);
static int kiki_pwd_mng_kiPasswordMCryptEncryptor_decrypt(kiPasswordDecryptor* _self, void* memory,
                                                          size_t input_length);
static int kiki_pwd_mng_kiPasswordMCryptEncryptor_set_iv(kiPasswordEncryptor* _self, IV_t);
static int kiki_pwd_mng_kiPasswordMCryptEncryptor_set_secret_key(kiPasswordEncryptor* _self, kiki_pwd_mng_secret_key_t);

kiPasswordMCryptEncryptor* kiki_pwd_mng_kiPasswordMCryptEncryptor_init(kiPasswordMCryptEncryptor* _self) {
	kiPasswordMCryptEncryptor* self = _self;
	if(!_self) { self = (kiPasswordMCryptEncryptor*) malloc(sizeof(kiPasswordMCryptEncryptor)); }

	kiki_pwd_mng_secret_key_t_init(&self->secret_key);
	memset(self->iv, 0, 16);

	self->destroy = &kiki_pwd_mng_kiPasswordMCryptEncryptor_destroy;

	self->encryptor.object = (void*) self;
	self->encryptor.crypt = &kiki_pwd_mng_kiPasswordMCryptEncryptor_crypt;
	self->encryptor.set_iv = &kiki_pwd_mng_kiPasswordMCryptEncryptor_set_iv;
	self->encryptor.set_secret_key = &kiki_pwd_mng_kiPasswordMCryptEncryptor_set_secret_key;

	self->decryptor.object = (void*) self;
	self->decryptor.decrypt = &kiki_pwd_mng_kiPasswordMCryptEncryptor_decrypt;
	self->decryptor.set_iv = (int (*)(kiPasswordDecryptor*,IV_t))&kiki_pwd_mng_kiPasswordMCryptEncryptor_set_iv;
	self->decryptor.set_secret_key = (int (*)(kiPasswordDecryptor*,kiki_pwd_mng_secret_key_t))&kiki_pwd_mng_kiPasswordMCryptEncryptor_set_secret_key;

	return self;
}

void kiki_pwd_mng_kiPasswordMCryptEncryptor_free(kiPasswordMCryptEncryptor* self) {
	self->destroy(self);
	free(self);
}

//methods

void kiki_pwd_mng_kiPasswordMCryptEncryptor_destroy(kiPasswordMCryptEncryptor* self) {
	self->secret_key.destroy(&self->secret_key);
}

int kiki_pwd_mng_kiPasswordMCryptEncryptor_crypt(kiPasswordEncryptor* _self, void* memory,
                                                 size_t input_length) {
	kiPasswordMCryptEncryptor* self = (kiPasswordMCryptEncryptor*) _self->object;
	if(!self->secret_key.length) return -1;
	if(!self->iv[0]) return -2;

	MCRYPT mcrypt_thread_descriptor = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
	int block_size = mcrypt_enc_get_block_size(mcrypt_thread_descriptor);
	while(input_length % block_size) { ++input_length; }

	mcrypt_generic_init(mcrypt_thread_descriptor, self->secret_key.value, self->secret_key.length, self->iv);
	mcrypt_generic(mcrypt_thread_descriptor, memory, input_length);
	mcrypt_generic_deinit(mcrypt_thread_descriptor);
	mcrypt_module_close(mcrypt_thread_descriptor);

	return 0;
}

int kiki_pwd_mng_kiPasswordMCryptEncryptor_decrypt(kiPasswordDecryptor* _self, void* memory,
                                                   size_t input_length) {
	kiPasswordMCryptEncryptor* self = (kiPasswordMCryptEncryptor*) _self->object;
	if(!self->secret_key.length) return -1;
	if(!self->iv[0]) return -2;

	MCRYPT mcrypt_thread_descriptor = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
	int block_size = mcrypt_enc_get_block_size(mcrypt_thread_descriptor);
	if(input_length % block_size != 0) { return -3; }

	mcrypt_generic_init(mcrypt_thread_descriptor, self->secret_key.value, self->secret_key.length, self->iv);
	mdecrypt_generic(mcrypt_thread_descriptor, memory, input_length);
	mcrypt_generic_deinit(mcrypt_thread_descriptor);
	mcrypt_module_close(mcrypt_thread_descriptor);

	return 0;
}

int kiki_pwd_mng_kiPasswordMCryptEncryptor_set_iv(kiPasswordEncryptor* _self, IV_t iv) {
	kiPasswordMCryptEncryptor* self = (kiPasswordMCryptEncryptor*) _self->object;
	memcpy(self->iv, iv, 16);
	return 0;
}

int kiki_pwd_mng_kiPasswordMCryptEncryptor_set_secret_key(kiPasswordEncryptor* _self,
                                                          kiki_pwd_mng_secret_key_t secret_key) {
	kiPasswordMCryptEncryptor* self = (kiPasswordMCryptEncryptor*) _self->object;

	if(self->secret_key.value == secret_key.value) { return -1; }

	self->secret_key.destroy(&self->secret_key);
	self->secret_key = secret_key;
	return 0;
}