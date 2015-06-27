#ifndef BLURGATHER_SECRET_KEY_H
#define BLURGATHER_SECRET_KEY_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bg_secret_key;
typedef struct bg_secret_key bg_secret_key;

struct bg_secret_key {
	void (* destroy)(bg_secret_key* self);

	size_t length;
	unsigned char* value;

	int (* update)(bg_secret_key* self, const void* value, size_t length);
};

bg_secret_key* bg_secret_key_init(bg_secret_key* self);
void bg_secret_key_free(bg_secret_key* self);

#ifdef __cplusplus
}
#endif

#endif //BLURGATHER_SECRET_KEY_H
