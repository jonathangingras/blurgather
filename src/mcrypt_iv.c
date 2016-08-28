#include "blurgather/mcrypt_iv.h"
#include <stdio.h>

static void bg_mcrypt_iv_destroy(IV_t* _iv) {
	free(_iv->value);
}

static int bg_mcrypt_iv_generate(IV_t* _iv) {
  FILE* urandom = fopen("/dev/urandom", "rb");
  if(!urandom) { return -1; }
  
  fread(_iv->value, sizeof(unsigned char), _iv->length, urandom);

  fclose(urandom);
  return 0;
}

IV_t* bg_mcrypt_iv_init(IV_t* _iv) {
  IV_t* iv = _iv ? _iv : (IV_t*)malloc(sizeof(IV_t));
  iv->length = 32;
  iv->value = (unsigned char*)calloc(iv->length, sizeof(unsigned char));
  iv->destroy = &bg_mcrypt_iv_destroy;
  iv->randomize = &bg_mcrypt_iv_generate;
  return iv;
}
