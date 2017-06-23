#include <blurgather/map.h>
#include <sweetgreen/sweetgreen.h>

bg_map *map;

sweetgreen_setup(map) {
  map = bg_map_new();
}

sweetgreen_teardown(map) {
  bg_map_free(map);
}


sweetgreen_test_define(map, can_register_a_pair) {
  int i;

  bg_map_register_data(map, bg_string_from_str("somekey"), &i);

  sweetgreen_expect_same_address(&i, bg_map_get_data(map, bg_string_from_str("somekey")));
}

sweetgreen_test_define(map, getting_non_existent_key_returns_null_when_empty) {
  sweetgreen_expect_null(bg_map_get_data(map, bg_string_from_str("somekey")));
}

sweetgreen_test_define(map, getting_non_existent_key_returns_null_when_non_empty) {
  int i, j, k;

  bg_map_register_data(map, bg_string_from_str("somekey1"), &i);
  bg_map_register_data(map, bg_string_from_str("somekey2"), &j);
  bg_map_register_data(map, bg_string_from_str("somekey3"), &k);

  sweetgreen_expect_null(bg_map_get_data(map, bg_string_from_str("somekey")));
}

sweetgreen_test_define(map, can_reregister_a_pair_using_same_key) {
  int i, j;
  bg_map_register_data(map, bg_string_from_str("somekey"), &i);

  bg_map_register_data(map, bg_string_from_str("somekey"), &j);

  sweetgreen_expect_same_address(&j, bg_map_get_data(map, bg_string_from_str("somekey")));
}

sweetgreen_test_define(map, can_register_multiple_pairs) {
  int i, j, k;

  bg_map_register_data(map, bg_string_from_str("somekey1"), &i);
  bg_map_register_data(map, bg_string_from_str("somekey2"), &j);
  bg_map_register_data(map, bg_string_from_str("somekey3"), &k);

  sweetgreen_expect_same_address(&i, bg_map_get_data(map, bg_string_from_str("somekey1")));
  sweetgreen_expect_same_address(&j, bg_map_get_data(map, bg_string_from_str("somekey2")));
  sweetgreen_expect_same_address(&k, bg_map_get_data(map, bg_string_from_str("somekey3")));
}

const bg_string *keys[3] = {
  NULL, NULL, NULL
};
void *values[3] = {
  NULL, NULL, NULL
};
size_t times_called = 0;
int on_pair(const bg_string *key, void *value, void* output) {
  keys[times_called] = key;
  values[times_called] = value;
  times_called++;
  return 0;
}

sweetgreen_test_define(map, can_iterate_over_pairs) {
  int i, j, k;
  bg_map_register_data(map, bg_string_from_str("somekey1"), &i);
  bg_map_register_data(map, bg_string_from_str("somekey2"), &j);
  bg_map_register_data(map, bg_string_from_str("somekey3"), &k);

  bg_map_foreach(map, &on_pair, NULL);

  sweetgreen_expect_equal_string("somekey1", bg_string_data(keys[0]));
  sweetgreen_expect_equal_string("somekey2", bg_string_data(keys[1]));
  sweetgreen_expect_equal_string("somekey3", bg_string_data(keys[2]));
  sweetgreen_expect_same_address(&i, values[0]);
  sweetgreen_expect_same_address(&j, values[1]);
  sweetgreen_expect_same_address(&k, values[2]);
}