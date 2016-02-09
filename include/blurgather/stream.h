#ifndef BLURGATHER_STREAM_H
#define BLURGATHER_STREAM_H

#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bg_stream;
typedef struct bg_stream bg_stream;

typedef int bg_stream_mode;
#define BGSM_READ   1
#define BGSM_WRITE  2
#define BGSM_APPEND 4

typedef int bg_stream_object;
#define BGSO_FILE  1
#define BGSO_MEM   2

bg_stream *bg_stream_open_(bg_stream_object oflag, bg_stream_mode mflags, ...);
#define bg_stream_open(...) bg_stream_open_(__VA_ARGS__, NULL)

void bg_stream_close(bg_stream *stream);
void bg_stream_rewind(bg_stream *stream);
void bg_stream_forward(bg_stream *stream);
size_t bg_stream_write(bg_stream *stream, const void *data, size_t size);
size_t bg_stream_read(bg_stream *stream, void *data, size_t size);
size_t bg_stream_length(bg_stream *stream);

#ifdef __cplusplus
}
#endif

#endif
