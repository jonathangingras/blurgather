#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stream.h"

struct bg_stream_vtable {
  void (* close)(bg_stream *stream);
  void (* rewind)(bg_stream *stream);
  void (* forward)(bg_stream *stream);
  size_t (* write)(bg_stream *stream, const void *data, size_t size);
  size_t (* read)(bg_stream *stream, void *data, size_t size);
  size_t (* length)(bg_stream *stream);
};

struct bg_stream {
  void *object;
  struct bg_stream_vtable *vtable;

  bg_stream_mode mode;
  void (* at_close)(void *);
  void *at_close_arg;
};

static void bg_stream_at_close(bg_stream *stream) {
  if(stream->at_close) {
    stream->at_close(stream->at_close_arg);
  }
}

/* memory implementation */
typedef struct {
  unsigned char *data;
  size_t offset;
  size_t allocated;
  size_t length;
} bg_mem_stream;

static void bg_mem_stream_close(bg_stream *_stream) {
  bg_mem_stream *stream = (bg_mem_stream *)_stream->object;
  
  bg_stream_at_close(_stream);

  free(stream->data);
  free(stream);
  free(_stream);
}

static void bg_mem_stream_rewind(bg_stream *_stream) {
  bg_mem_stream *stream = (bg_mem_stream *)_stream->object;

  stream->offset = 0;
}

static void bg_mem_stream_forward(bg_stream *_stream) {
  bg_mem_stream *stream = (bg_mem_stream *)_stream->object;

  stream->offset = stream->length;
}

static void *bg_mem_stream_grow(bg_mem_stream *mem_stream) {
  return ((mem_stream->data = realloc(mem_stream->data, (mem_stream->allocated += 1024))));
}

static size_t bg_mem_stream_write(bg_stream *_stream, const void *data, size_t size) {
  bg_mem_stream *stream = (bg_mem_stream *)_stream->object;

  while(stream->offset + size > stream->allocated) {
    bg_mem_stream_grow(stream);
  }

  memcpy(stream->data + stream->offset, data, size);
  stream->offset += size;
  if(stream->offset > stream->length) {
    stream->length = stream->offset;
  }

  return size;
}

static size_t bg_mem_stream_read(bg_stream *_stream, void *data, size_t size) {
  bg_mem_stream *stream = (bg_mem_stream *)_stream->object;

  if(stream->offset + size > stream->length) {
    size = stream->length - stream->offset;
    
    if(size == 0) {
      return 0;
    }
  }

  memcpy(data, stream->data + stream->offset, size);
  stream->offset += size;

  return size;
}

static size_t bg_mem_stream_length(bg_stream *_stream) {
  bg_mem_stream *stream = (bg_mem_stream *)_stream->object;

  return stream->length;
}

static struct bg_stream_vtable bg_mem_stream_vtable = {
  .close = &bg_mem_stream_close,
  .rewind = &bg_mem_stream_rewind,
  .forward = &bg_mem_stream_forward,
  .write = &bg_mem_stream_write,
  .read = &bg_mem_stream_read,
  .length = &bg_mem_stream_length
};

static void bg_stream_open_mem(bg_stream *stream, va_list vl) {
  stream->vtable = &bg_mem_stream_vtable;
  stream->object = malloc(sizeof(bg_mem_stream));
  
  bg_mem_stream *object = stream->object;
  object->data = NULL;
  object->allocated = 0;
  object->length = 0;
  object->offset = 0;

  bg_mem_stream_grow(object);
}

/* file stream implementation */
typedef struct {
  char *filename;
  FILE *file;
} bg_file_stream;

static void bg_file_stream_close(bg_stream *_stream) {
  bg_file_stream *stream = (bg_file_stream *)_stream->object;

  fflush(stream->file);
  fclose(stream->file);

  bg_stream_at_close(_stream);

  free(stream->filename);
  free(stream);
  free(_stream);
}

static void bg_file_stream_rewind(bg_stream *_stream) {
  bg_file_stream *stream = (bg_file_stream *)_stream->object;

  fflush(stream->file);
  fseek(stream->file, 0L, SEEK_SET);
}

static void bg_file_stream_forward(bg_stream *_stream) {
  bg_file_stream *stream = (bg_file_stream *)_stream->object;

  fflush(stream->file);
  fseek(stream->file, 0L, SEEK_END);
}

static size_t bg_file_stream_write(bg_stream *_stream, const void *data, size_t size) {
  bg_file_stream *stream = (bg_file_stream *)_stream->object;

  return fwrite(data, sizeof(char), size, stream->file);
}

static size_t bg_file_stream_read(bg_stream *_stream, void *data, size_t size) {
  bg_file_stream *stream = (bg_file_stream *)_stream->object;

  return fread(data, sizeof(char), size, stream->file);
}

static size_t bg_file_stream_length(bg_stream *_stream) {
  bg_file_stream *stream = (bg_file_stream *)_stream->object;

  size_t length, position;
  position = ftell(stream->file);
  
  fseek(stream->file, 0L, SEEK_SET);
  fseek(stream->file, 0L, SEEK_END);
  length = ftell(stream->file);
  fseek(stream->file, position, SEEK_SET);

  return length;
}

static struct bg_stream_vtable bg_file_stream_vtable = {
  .close = &bg_file_stream_close,
  .rewind = &bg_file_stream_rewind,
  .forward = &bg_file_stream_forward,
  .write = &bg_file_stream_write,
  .read = &bg_file_stream_read,
  .length = &bg_file_stream_length
};

static void bg_stream_open_file(bg_stream *stream, bg_stream_mode mflags, va_list vl) {
  stream->vtable = &bg_file_stream_vtable;
  stream->object = malloc(sizeof(bg_file_stream));
  
  bg_file_stream *object = stream->object;
  object->filename = strdup(va_arg(vl, const char *));

  object->file = fopen(object->filename, "ab+");
}

void bg_stream_register_at_close(bg_stream *stream, va_list vl) {
  void (* at_close)(void *) = va_arg(vl, void (*)(void *));
  if(at_close) {
    stream->at_close = at_close;

    void *at_close_arg = va_arg(vl, void *);
    if(at_close_arg) {
      stream->at_close_arg = at_close_arg;
    }
  }
}

/* factory method */
bg_stream *bg_stream_open_(bg_stream_object oflag, bg_stream_mode mflags, ...) {
  if(__builtin_popcount(oflag) != 1) {
    return NULL;
  }

  bg_stream *stream = malloc(sizeof(bg_stream));
  stream->mode = mflags;
  stream->at_close = NULL;
  stream->at_close_arg = NULL;

  va_list vl;
  va_start(vl, mflags);
  if(oflag & BGSO_MEM) {
    bg_stream_open_mem(stream, vl);
  } else if(oflag & BGSO_FILE) {
    bg_stream_open_file(stream, mflags, vl);
  }
  bg_stream_register_at_close(stream, vl);
  va_end(vl);
  
  return stream;
}

/* abstract wrappers */
void bg_stream_close(bg_stream *stream) {
  return stream->vtable->close(stream);
}

void bg_stream_rewind(bg_stream *stream) {
  return stream->vtable->rewind(stream);
}

void bg_stream_forward(bg_stream *stream) {
  return stream->vtable->forward(stream);
}

size_t bg_stream_write(bg_stream *stream, const void *data, size_t size) {
  if(!(stream->mode & BGSM_WRITE)) {
    return 0;
  }

  return stream->vtable->write(stream, data, size);
}

size_t bg_stream_read(bg_stream *stream, void *data, size_t size) {
  if(!(stream->mode & BGSM_READ)) {
    return 0;
  }

  return stream->vtable->read(stream, data, size);
}

size_t bg_stream_length(bg_stream *stream) {
  return stream->vtable->length(stream);
}
