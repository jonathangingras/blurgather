#include <string.h>
#include "blurgather/iv.h"

int bg_iv_copy(IV_t *self, const IV_t *iv) {
  if(iv->length != self->length) {
    return -1;
  }
  if(!iv->value) {
    return -2;
  }

  memset(self->value, 0, self->length);
  memcpy(self->value, iv->value, self->length);

  return 0;
}
