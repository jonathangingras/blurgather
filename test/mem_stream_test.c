#include <string.h>

#include "stream.h"

#include <sweetgreen/sweetgreen.h>


sweetgreen_test_define(mem_stream, can_open_stream) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_WRITE | BGSM_READ);

  sweetgreen_expect_not_null(stream);
  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, can_write_to_writable_stream) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_WRITE);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;

  sweetgreen_expect_equal(data_length, bg_stream_write(stream, data, data_length));
  sweetgreen_expect_equal(data_length, bg_stream_length(stream));
  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, cannot_write_to_non_writable_stream) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_READ);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;

  sweetgreen_expect_equal(0, bg_stream_write(stream, data, data_length));
  sweetgreen_expect_equal(0, bg_stream_length(stream));
  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, write_to_writable_stream_writes_good_value) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_WRITE | BGSM_READ);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  
  char read_data[data_length];
  sweetgreen_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  sweetgreen_expect_equal_memory(data, read_data, data_length);
  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, cannot_read_to_non_readable_stream) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_WRITE);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  
  char read_data[data_length];
  sweetgreen_expect_equal(0, bg_stream_read(stream, read_data, data_length));
  sweetgreen_expect_not_equal_memory(data, read_data, data_length);
  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, writing_0_has_no_effect) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_READ | BGSM_WRITE);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;
  char read_data1[data_length];
  char read_data2[data_length];
  memcpy(read_data2, read_data1, data_length);

  bg_stream_write(stream, data, 0);
  bg_stream_rewind(stream);
  
  sweetgreen_expect_equal(0, bg_stream_read(stream, read_data2, data_length));
  sweetgreen_expect_equal_memory(read_data1, read_data2, data_length);
  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, reading_0_has_no_effect) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_READ | BGSM_WRITE);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;
  char read_data1[data_length];
  char read_data2[data_length];
  memcpy(read_data2, read_data1, data_length);
  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  
  sweetgreen_expect_equal(0, bg_stream_read(stream, read_data2, 0));
  sweetgreen_expect_equal_memory(read_data1, read_data2, data_length);

  sweetgreen_expect_equal(data_length, bg_stream_read(stream, read_data2, data_length));
  sweetgreen_expect_equal_memory(data, read_data2, data_length);
  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, write_to_stream_twice_stores_data_in_good_order) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_WRITE | BGSM_READ);
  char data[] = "hello, it's me";
  size_t data_length = strlen(data) + 1;
  char data1[] = "hello, ", data2[] = "it's me";
  size_t data1_length = strlen(data1) + 1, data2_length = strlen(data2) + 1;

  bg_stream_write(stream, data1, data1_length - 1); // - 1, we dont wanna store '\0'
  bg_stream_write(stream, data2, data2_length);
  bg_stream_rewind(stream);
  
  char read_data[data_length];
  sweetgreen_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  sweetgreen_expect_equal_memory(data, read_data, data_length);
  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, write_to_stream_twice_stores_data_in_good_order_when_rewind_between_calls_case_offset_before_length) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_WRITE | BGSM_READ);
  char data[] = "it's meou, ";
  size_t data_length = strlen(data) + 1;
  char data1[] = "hello you, ", data2[] = "it's me";
  size_t data1_length = strlen(data1) + 1, data2_length = strlen(data2) + 1;

  bg_stream_write(stream, data1, data1_length);
  bg_stream_rewind(stream);
  bg_stream_write(stream, data2, data2_length - 1); // - 1, we dont wanna store '\0'
  bg_stream_rewind(stream);
  
  char read_data[data_length];
  sweetgreen_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  sweetgreen_expect_equal(data_length, bg_stream_length(stream));
  sweetgreen_expect_equal_memory(data, read_data, data_length);
  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, write_to_stream_twice_stores_data_in_good_order_when_rewind_between_calls_case_offset_after_length) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_WRITE | BGSM_READ);
  char data[] = "it's me, I was wondering";
  size_t data_length = strlen(data) + 1;
  char data1[] = "hello you, ", data2[] = "it's me, I was wondering";
  size_t data1_length = strlen(data1) + 1, data2_length = strlen(data2) + 1;

  bg_stream_write(stream, data1, data1_length);
  bg_stream_rewind(stream);
  bg_stream_write(stream, data2, data2_length); // we wanna store '\0'
  bg_stream_rewind(stream);
  
  char read_data[data_length];
  sweetgreen_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  sweetgreen_expect_equal(data_length, bg_stream_length(stream));
  sweetgreen_expect_equal_memory(data, read_data, data_length);
  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, write_to_stream_twice_stores_data_in_good_order_when_rewind_and_forward_between_calls_case_offset_before_length) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_WRITE | BGSM_READ);
  char data[] = "it's meou, it's me";
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
  sweetgreen_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  sweetgreen_expect_equal(data_length, bg_stream_length(stream));
  sweetgreen_expect_equal_memory(data, read_data, data_length);
  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, allocation_is_dynamic_when_write_512b_buffer) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_WRITE | BGSM_READ);
  size_t data_length = 512;
  char *data = malloc(data_length);
  char *read_data = malloc(data_length);
  int urandom = open("/dev/urandom", O_RDONLY);
  read(urandom, data, data_length);
  close(urandom);

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  sweetgreen_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  sweetgreen_expect_equal(data_length, bg_stream_length(stream));
  sweetgreen_expect_equal_memory(data, read_data, data_length);

  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, allocation_is_dynamic_when_write_1024b_buffer) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_WRITE | BGSM_READ);
  size_t data_length = 1024;
  char *data = malloc(data_length);
  char *read_data = malloc(data_length);
  int urandom = open("/dev/urandom", O_RDONLY);
  read(urandom, data, data_length);
  close(urandom);

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  sweetgreen_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  sweetgreen_expect_equal(data_length, bg_stream_length(stream));
  sweetgreen_expect_equal_memory(data, read_data, data_length);

  free(data);
  free(read_data);
  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, allocation_is_dynamic_when_write_1536b_buffer) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_WRITE | BGSM_READ);
  size_t data_length = 1536;
  char *data = malloc(data_length);
  char *read_data = malloc(data_length);
  int urandom = open("/dev/urandom", O_RDONLY);
  read(urandom, data, data_length);
  close(urandom);

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  sweetgreen_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  sweetgreen_expect_equal(data_length, bg_stream_length(stream));
  sweetgreen_expect_equal_memory(data, read_data, data_length);

  free(data);
  free(read_data);
  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, allocation_is_dynamic_when_write_2048b_buffer) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_WRITE | BGSM_READ);
  size_t data_length = 2048;
  char *data = malloc(data_length);
  char *read_data = malloc(data_length);
  int urandom = open("/dev/urandom", O_RDONLY);
  read(urandom, data, data_length);
  close(urandom);

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  sweetgreen_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  sweetgreen_expect_equal(data_length, bg_stream_length(stream));
  sweetgreen_expect_equal_memory(data, read_data, data_length);

  free(data);
  free(read_data);
  bg_stream_close(stream);
}

sweetgreen_test_define(mem_stream, allocation_is_dynamic_when_write_4194304b_buffer) {
  bg_stream *stream = bg_stream_open(BGSO_MEM, BGSM_WRITE | BGSM_READ);
  size_t data_length = 4194304;
  char *data = malloc(data_length);
  char *read_data = malloc(data_length);
  int urandom = open("/dev/urandom", O_RDONLY);
  read(urandom, data, data_length);
  close(urandom);

  bg_stream_write(stream, data, data_length);
  bg_stream_rewind(stream);
  sweetgreen_expect_equal(data_length, bg_stream_read(stream, read_data, data_length));
  sweetgreen_expect_equal(data_length, bg_stream_length(stream));
  sweetgreen_expect_equal_memory(data, read_data, data_length);

  free(data);
  free(read_data);
  bg_stream_close(stream);
}
