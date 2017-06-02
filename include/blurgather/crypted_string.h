#ifndef BLURGATHER_CRYPTED_STRING_H
#define BLURGATHER_CRYPTED_STRING_H

#include "string.h"
#include "cryptor.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_crypted_string;
typedef struct bg_crypted_string bg_crypted_string;

bg_string *bg_crypted_string_decrypt(const bg_crypted_string *cr_str, const bg_cryptor_t *cryptor, const bg_secret_key_t *key);
bg_crypted_string *bg_crypted_string_new(const bg_string *str, const bg_cryptor_t *cryptor, const bg_secret_key_t *key);

#ifdef __cplusplus
}
#endif

#endif
