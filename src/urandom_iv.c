#include <unistd.h>
#include <sys/fcntl.h>

#include "blurgather/urandom_iv.h"

int bg_get_devurandom_iv(unsigned char *iv, size_t length) {
    int urandom = open("/dev/urandom", O_RDONLY);
    size_t read_ = read(urandom, iv, length);
    close(urandom);
    return read_ != length;
}
