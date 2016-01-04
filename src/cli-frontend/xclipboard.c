#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "password.h"
#include "utils.h"

void send_to_clipboard(const char* password) {
	init_char_array(command, BLURGATHER_PWD_MAX_VALUE_LEN + 40);
	strcat(command, "echo -n '");
	printf("%s", password);
	strcat(command, "' | xclip -selection clipboard -i");
	system(command);
}
