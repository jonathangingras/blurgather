#ifndef BLURGATHER_PASSWORD_H
#define BLURGATHER_PASSWORD_H

#include <string.h>
#include <stdlib.h>

#include "context.h"
#include "string.h"
#include "iv.h"

#ifdef __cplusplus
extern "C" {
#endif

/* constructor */
bg_password *bg_password_new(void);

/* deserialize */
int bg_password_fill_raw(bg_password *password, const void *crypted_value, size_t crypted_value_size);

/* structure size */
size_t bg_password_size();

/* destroy and free */
void bg_password_destroy(bg_password *password);
void bg_password_free(bg_password *password);

/* deep copy */
bg_password *bg_password_copy(const bg_password *password);

/* name */
const bg_string *bg_password_name(bg_password *password);
int bg_password_update_name(bg_password *password, bg_string *name);

/* description */
const bg_string *bg_password_description(bg_password *password);
int bg_password_update_description(bg_password *password, bg_string *description);

/* value */
const bg_string *bg_password_value(bg_password *password);
size_t bg_password_value_length(bg_password *password);
int bg_password_update_value(bg_password *password, bg_string *value);

/* crypted flag */
int bg_password_crypted(bg_password *password);

/* initialization vector object */
//const bg_iv_t *bg_password_iv(bg_password *password);

/* methods */
int bg_password_crypt(bg_password* password, bg_cryptor_t *cryptor, bg_secret_key_t *key);
int bg_password_decrypt(bg_password* password, bg_cryptor_t *cryptor, bg_secret_key_t *key);


#ifdef __cplusplus
}
#endif

#endif /* BLURGATHER_PASSWORD_H */
