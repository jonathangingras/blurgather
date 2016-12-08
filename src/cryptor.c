#include <blurgather/cryptor.h>

int bg_cryptor_encrypt(const bg_cryptor_t *cryptor, void *memory, size_t memlen, const bg_secret_key_t *secret_key, const bg_iv_t *iv) {
    return cryptor->encrypt(memory, memlen, secret_key, iv);
}

int bg_cryptor_decrypt(const bg_cryptor_t *cryptor, void *memory, size_t memlen, const bg_secret_key_t *secret_key, const bg_iv_t *iv) {
    return cryptor->decrypt(memory, memlen, secret_key, iv);
}

size_t bg_cryptor_iv_length(const bg_cryptor_t *cryptor) {
    return cryptor->iv_length();
}

int bg_cryptor_generate_iv(const bg_cryptor_t *cryptor, bg_iv_t **output) {
    return cryptor->generate_iv(output);
}
