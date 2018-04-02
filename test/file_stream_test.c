#include <prufen/prufen.h>
#include <string.h>
#include "blurgather/stream.h"

const char *filename = "file_stream.txt";

void at_close(void *arg) {
  remove((const char*)arg);
}

pruf_test_define(file_stream, can_open_stream) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE | BGSM_READ, filename, &at_close, filename);

  pruf_expect_not_null(stream);
  bg_stream_close(stream);
}

pruf_test_define(file_stream, can_write_to_writable_stream) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE, filename, &at_close, filename);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;

  pruf_expect_equal(data_length, bg_stream_write(stream, data, data_length));
  pruf_expect_equal(data_length, bg_stream_length(stream));
  bg_stream_close(stream);
}

pruf_test_define(file_stream, cannot_write_to_non_writable_stream) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_READ, filename, &at_close, filename);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;

  pruf_expect_equal(0, bg_stream_write(stream, data, data_length));
  pruf_expect_equal(0, bg_stream_length(stream));
  bg_stream_close(stream);
}

pruf_test_define(file_stream, write_to_writable_stream_writes_good_value) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE | BGSM_READ, filename, &at_close, filename);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  
  char read_data[data_length];
  pruf_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  pruf_expect_equal_memory(data, read_data, data_length);
  bg_stream_close(stream);
}

pruf_test_define(file_stream, cannot_read_to_non_readable_stream) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE, filename, &at_close, filename);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  
  char read_data[data_length];
  pruf_expect_equal(0, bg_stream_read(stream, read_data, data_length));
  pruf_expect_not_equal_memory(data, read_data, data_length);
  bg_stream_close(stream);
}

pruf_test_define(file_stream, writing_0_has_no_effect) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_READ | BGSM_WRITE, filename, &at_close, filename);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;
  char read_data1[data_length];
  char read_data2[data_length];
  memcpy(read_data2, read_data1, data_length);

  bg_stream_write(stream, data, 0);
  bg_stream_rewind(stream);
  
  pruf_expect_equal(0, bg_stream_read(stream, read_data2, data_length));
  pruf_expect_equal_memory(read_data1, read_data2, data_length);
  bg_stream_close(stream);
}

pruf_test_define(file_stream, reading_0_has_no_effect) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_READ | BGSM_WRITE, filename, &at_close, filename);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;
  char read_data1[data_length];
  char read_data2[data_length];
  memcpy(read_data2, read_data1, data_length);
  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  
  pruf_expect_equal(0, bg_stream_read(stream, read_data2, 0));
  pruf_expect_equal_memory(read_data1, read_data2, data_length);

  pruf_expect_equal(data_length, bg_stream_read(stream, read_data2, data_length));
  pruf_expect_equal_memory(data, read_data2, data_length);
  bg_stream_close(stream);
}

pruf_test_define(file_stream, write_to_stream_twice_stores_data_in_good_order) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE | BGSM_READ, filename, &at_close, filename);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;
  char data1[] = "hello, ", data2[] = "it's me";
  size_t data1_length = strlen(data1) + 1, data2_length = strlen(data2) + 1;

  bg_stream_write(stream, data1, data1_length - 1); // - 1, we dont wanna store '\0'
  bg_stream_write(stream, data2, data2_length);
  bg_stream_rewind(stream);
  
  char read_data[data_length];
  pruf_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  pruf_expect_equal_memory(data, read_data, data_length);
  bg_stream_close(stream);
}

pruf_test_define(file_stream, cannot_read_when_no_prior_rewind) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE | BGSM_READ, filename, &at_close, filename);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;

  bg_stream_write(stream, data, data_length);
  
  char read_data[data_length];
  pruf_expect_zero(bg_stream_read(stream, read_data, data_length));
  pruf_expect_not_equal_memory(data, read_data, data_length);
  bg_stream_close(stream);
}

pruf_test_define(file_stream, can_read_when_prior_rewind) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE | BGSM_READ, filename, &at_close, filename);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  
  char read_data[data_length];
  pruf_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  pruf_expect_equal_memory(data, read_data, data_length);
  bg_stream_close(stream);
}

pruf_test_define(file_stream, reading_is_contiguous_when_rewinding) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE | BGSM_READ, filename, &at_close, filename);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;
  char data1[] = "hello, ", data2[] = "it's me";
  size_t data1_length = strlen(data1) + 1, data2_length = strlen(data2) + 1;

  bg_stream_write(stream, data1, data1_length - 1);
  bg_stream_write(stream, data2, data2_length);
  bg_stream_rewind(stream);
  
  char read_data[data_length];
  pruf_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  pruf_expect_equal_memory(data, read_data, data_length);
  bg_stream_close(stream);
}

pruf_test_define(file_stream, when_rewind_between_write_calls_then_rewind_has_no_effect_to_write__case_offset_before_length) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE | BGSM_READ, filename, &at_close, filename);
  char data[] = "hello you, it's me";
  size_t data_length = strlen(data) + 1;
  char data1[] = "hello you, ", data2[] = "it's me";
  size_t data1_length = strlen(data1) + 1, data2_length = strlen(data2) + 1;

  bg_stream_write(stream, data1, data1_length - 1); // - 1, we dont wanna store '\0'
  bg_stream_rewind(stream);
  bg_stream_write(stream, data2, data2_length);
  bg_stream_rewind(stream);
  
  char read_data[data_length];
  pruf_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  pruf_expect_equal(data_length, bg_stream_length(stream));
  pruf_expect_equal_memory(data, read_data, data_length);
  bg_stream_close(stream);
}

pruf_test_define(file_stream, when_rewind_and_forward_between_write_calls_then_rewind_and_forward_have_no_effect_to_write) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE | BGSM_READ, filename, &at_close, filename);
  char data[] = "hello you, it's meit's me";
  size_t data_length = strlen(data) + 1;
  char data1[] = "hello you, ", data2[] = "it's me";
  size_t data1_length = strlen(data1) + 1, data2_length = strlen(data2) + 1;

  bg_stream_write(stream, data1, data1_length - 1);
  bg_stream_rewind(stream);
  bg_stream_write(stream, data2, data2_length - 1);
  bg_stream_forward(stream);
  bg_stream_write(stream, data2, data2_length); // we wanna store '\0'
  bg_stream_rewind(stream);
  
  char read_data[data_length];
  pruf_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  pruf_expect_equal(data_length, bg_stream_length(stream));
  pruf_expect_equal_memory(data, read_data, data_length);
  bg_stream_close(stream);
}

pruf_test_define(file_stream, allocation_is_dynamic_when_write_512b_buffer) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE | BGSM_READ, filename, &at_close, filename);
  size_t data_length = 512;
  char *data = malloc(data_length);
  char *read_data = malloc(data_length);
  int urandom = open("/dev/urandom", O_RDONLY);
  read(urandom, data, data_length);
  close(urandom);

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  pruf_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  pruf_expect_equal(data_length, bg_stream_length(stream));
  pruf_expect_equal_memory(data, read_data, data_length);

  bg_stream_close(stream);
}

pruf_test_define(file_stream, allocation_is_dynamic_when_write_1024b_buffer) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE | BGSM_READ, filename, &at_close, filename);
  size_t data_length = 1024;
  char *data = malloc(data_length);
  char *read_data = malloc(data_length);
  int urandom = open("/dev/urandom", O_RDONLY);
  read(urandom, data, data_length);
  close(urandom);

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  pruf_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  pruf_expect_equal(data_length, bg_stream_length(stream));
  pruf_expect_equal_memory(data, read_data, data_length);

  free(data);
  free(read_data);
  bg_stream_close(stream);
}

pruf_test_define(file_stream, allocation_is_dynamic_when_write_1536b_buffer) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE | BGSM_READ, filename, &at_close, filename);
  size_t data_length = 1536;
  char *data = malloc(data_length);
  char *read_data = malloc(data_length);
  int urandom = open("/dev/urandom", O_RDONLY);
  read(urandom, data, data_length);
  close(urandom);

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  pruf_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  pruf_expect_equal(data_length, bg_stream_length(stream));
  pruf_expect_equal_memory(data, read_data, data_length);

  free(data);
  free(read_data);
  bg_stream_close(stream);
}

pruf_test_define(file_stream, allocation_is_dynamic_when_write_2048b_buffer) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE | BGSM_READ, filename, &at_close, filename);
  size_t data_length = 2048;
  char *data = malloc(data_length);
  char *read_data = malloc(data_length);
  int urandom = open("/dev/urandom", O_RDONLY);
  read(urandom, data, data_length);
  close(urandom);

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  pruf_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  pruf_expect_equal(data_length, bg_stream_length(stream));
  pruf_expect_equal_memory(data, read_data, data_length);

  free(data);
  free(read_data);
  bg_stream_close(stream);
}

pruf_test_define(file_stream, allocation_is_dynamic_when_write_4194304b_buffer) {
  bg_stream *stream = bg_stream_open(BGSO_FILE, BGSM_WRITE | BGSM_READ, filename, &at_close, filename);
  size_t data_length = 4194304;
  char *data = malloc(data_length);
  char *read_data = malloc(data_length);
  int urandom = open("/dev/urandom", O_RDONLY);
  read(urandom, data, data_length);
  close(urandom);

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  pruf_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  pruf_expect_equal(data_length, bg_stream_length(stream));
  pruf_expect_equal_memory(data, read_data, data_length);

  free(data);
  free(read_data);
  bg_stream_close(stream);
}
