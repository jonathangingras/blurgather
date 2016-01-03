#ifndef BLURGATHER_PASSWORD_H
#define BLURGATHER_PASSWORD_H

#include <string.h>
#include <stdlib.h>

#include "iv.h"
#include "secret_key.h"
#include "encryptor.h"
#include "decryptor.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_password;
typedef struct bg_password bg_password;

#include "password_repository.h"

#define BLURGATHER_PWD_MAX_NAME_LEN 100
#define BLURGATHER_PWD_MAX_DESCRIPTION_LEN 500
#define BLURGATHER_PWD_MAX_VALUE_LEN 256

/* constructor */
bg_password* bg_password_init(bg_password* _self, IV_init_callback iv_initializer, bg_encryptor* encryptor, bg_decryptor* decryptor, bg_password_repository* repository);

/* deserialize */
int bg_password_fill_raw(bg_password *password, const IV_t *iv, const void* crypted_value, size_t crypted_value_size);

/* structure size */
size_t bg_password_size();

/* destroy and free */
void bg_password_destroy(bg_password *password);
void bg_password_free(bg_password *password);

/* name */
const char *bg_password_name(bg_password *password);
int bg_password_update_name(bg_password *password, const char *name);

/* description */
const char *bg_password_description(bg_password *password);
int bg_password_update_description(bg_password *password, const char *description);

/* value */
const char *bg_password_value(bg_password *password);
size_t bg_password_value_length(bg_password *password);
int bg_password_update_value(bg_password *password, const char *value);

/* crypted flag */
int bg_password_crypted(bg_password *password);

/* initialization vector object */
const unsigned char *bg_password_iv_value(bg_password *password);
size_t bg_password_iv_length(bg_password *password);

/* methods */
int bg_password_crypt(bg_password* password);
int bg_password_decrypt(bg_password* password);
int bg_password_save(bg_password* password);


#ifdef __cplusplus
}
#endif

#endif //BLURGATHER_PASSWORD_H
