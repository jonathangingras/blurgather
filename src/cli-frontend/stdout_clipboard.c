#include <stdio.h>

void send_to_stdout(const char* password) {
  printf("%s", password);
  fflush(stdout);
}
