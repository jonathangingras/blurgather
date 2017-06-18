#ifndef BLURGATHER_CRYPTOR_H
#define BLURGATHER_CRYPTOR_H

#include "iv.h"
#include "secret_key.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_cryptor_t {
  int (* const encrypt)(void *memory,
                        size_t memlen,
                        const bg_secret_key_t *secret_key,
                        const bg_iv_t *iv);

  int (* const decrypt)(void *memory,
                        size_t memlen,
                        const bg_secret_key_t *secret_key,
                        const bg_iv_t *iv);

  size_t (* const iv_length)();

  int (* const generate_iv)(bg_iv_t **output);

  size_t (* const encrypted_length)(size_t input_memlen);
};

/* abstract encryption */
int bg_cryptor_encrypt(const bg_cryptor_t *cryptor,
                       void *memory,
                       size_t memlen,
                       const bg_secret_key_t *secret_key,
                       const bg_iv_t *iv);

/* abstract decrytption */
int bg_cryptor_decrypt(const bg_cryptor_t *cryptor,
                       void *memory,
                       size_t memlen,
                       const bg_secret_key_t *secret_key,
                       const bg_iv_t *iv);

/* abstract algorithm's initialisation vector's or nounce's length */
size_t bg_cryptor_iv_length(const bg_cryptor_t *cryptor);

/* generate abstract algorithm's initialisation vector or nounce */
int bg_cryptor_generate_iv(const bg_cryptor_t *cryptor,
                           bg_iv_t **output);

/* memory length, in bytes, in which a raw byte array of length input_memlen
   will take when encrypted */
size_t bg_cryptor_encrypted_length(const bg_cryptor_t *cryptor, size_t input_memlen);

#ifdef __cplusplus
}
#endif

#endif /* BLURGATHER_CRYPTOR_H */
