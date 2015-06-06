#include "MCryptIV.h"
#include <stdlib.h>
#include <uuid/uuid.h>

static void kiki_pwd_mng_mcrypt_iv_destroy(IV_t* _iv) {
	free(_iv->value);
}

static int kiki_pwd_mng_mcrypt_iv_generate(IV_t* _iv) {
	uuid_generate(_iv->value);
	return 0;
}

IV_t* kiki_pwd_mng_mcrypt_iv_init(IV_t* _iv) {
	IV_t* iv = _iv ? _iv : (IV_t*)malloc(sizeof(IV_t));
	iv->length = 16;
	iv->value = (unsigned char*)calloc(iv->length, sizeof(unsigned char));
	iv->destroy = &kiki_pwd_mng_mcrypt_iv_destroy;
	iv->randomize = &kiki_pwd_mng_mcrypt_iv_generate;
	return iv;
}