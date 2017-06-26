#ifndef BLURGATHER_ENCRYPTION_H
#define BLURGATHER_ENCRYPTION_H

#include "string.h"
#include "cryptor.h"

#ifdef __cplusplus
extern "C" {
#endif

int bg_encrypt_string(bg_string **str, const bg_cryptor_t *cryptor, const bg_secret_key_t *key);
int bg_decrypt_string(bg_string **str, const bg_cryptor_t *cryptor, const bg_secret_key_t *key);

#ifdef __cplusplus
}
#endif

#endif
