#include <stdio.h>
#include <string.h>
#include <blurgather/secret_key.h>
#include "../blur.h"


int blur_unlock_from_stdin(bg_context *ctx, int argc, char **argv) {
  char buffer[1024];
  if(fgets(buffer, 1024, stdin) != buffer) {
    fprintf(stderr, "could not read from stdin!\n");
    return -1;
  }

  size_t buffer_len = strlen(buffer), i;
  for(i = 0; i < buffer_len; ++i) {
    if(buffer[i] == '\n') {
      buffer[i] = 0;
      break;
    }
  }

  bg_secret_key_t *master_key = bg_secret_key_new(buffer, buffer_len);
  if(bgctx_unlock(ctx, master_key)) {
    fprintf(stderr, "could not unlock context!\n");
    return -2;
  }

  return 0;
}
