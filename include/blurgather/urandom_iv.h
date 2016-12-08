#ifndef _BLURGATHER_URANDOM_IV_H_
#define _BLURGATHER_URANDOM_IV_H_

#ifdef __cplusplus
extern "C" {
#endif

int bg_get_devurandom_iv(unsigned char *iv, size_t length);

#ifdef __cplusplus
}
#endif

#endif
