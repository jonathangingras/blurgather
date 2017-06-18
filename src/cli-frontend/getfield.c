#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <blurgather/string.h>


bg_string *blur_getfield(const char* showing, int hide_input) {
  struct termios oflags, nflags;
  char buffer[2048];

  printf("%s: ", showing);
  fflush(stdout);

  if(hide_input) {
    tcgetattr(STDIN_FILENO, &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if(tcsetattr(STDIN_FILENO, TCSANOW, &nflags)) {
      return NULL;
    }
  }

  fgets(buffer, 2047, stdin);
  fflush(stdin);

  size_t i;
  for(i = 0; i < 2048; ++i) {
    if(buffer[i] == '\n') {
      buffer[i] = 0;
      break;
    }
  }

  if(hide_input) {
    if(tcsetattr(STDIN_FILENO, TCSANOW, &oflags)) {
      memset(buffer, 0, 2048);
      return NULL;
    }
  }

  return bg_string_from_str(buffer);
}
