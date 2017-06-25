#include <blurgather/map.h>

typedef void (*map_free_callback)(void *);

struct bg_map {
  size_t data_pair_count;
  bg_string **data_keys;
  void **data;
  map_free_callback *frees;
};

bg_map *bg_map_new() {
  bg_map *map = malloc(sizeof(bg_map));
  map->data_pair_count = 0;
  map->data_keys = NULL;
  map->data = NULL;
  map->frees = NULL;
  return map;
}

void bg_map_free(bg_map *map) {
  size_t i;
  for(i = 0; i < map->data_pair_count; ++i) {
    bg_string_free(map->data_keys[i]);
    if(map->frees[i]) {
      map->frees[i](map->data[i]);
    }
  }
  free(map->data_keys);
  free(map->data);
  free(map->frees);
  free(map);
}

static void get_data_index(bg_map *map, const bg_string *key, size_t *idx) {
  size_t i;
  for(i = 0; i < map->data_pair_count; ++i) {
    if(bg_string_compare(key, map->data_keys[i]) == 0) {
      *idx = i;
      return;
    }
  }
  *idx = map->data_pair_count;
}

int bg_map_register_data(bg_map *map, bg_string *key, void *value, void (*free_callback)(void *)) {
  size_t i;
  get_data_index(map, key, &i);

  if(!(i < map->data_pair_count)) { /* new key */
    map->data_keys = realloc(map->data_keys, (map->data_pair_count + 1) * sizeof(void*));
    map->data = realloc(map->data, (map->data_pair_count + 1) * sizeof(void*));
    map->frees = realloc(map->frees, (map->data_pair_count + 1) * sizeof(void*));

    map->data_keys[i] = key;
    map->data_pair_count++;
  } else { /* key already exists */
    bg_string_free(key);
    if(map->frees[i]) {
      map->frees[i](map->data[i]);
    }
  }

  map->data[i] = value;
  map->frees[i] = free_callback;

  return 0;
}

void *bg_map_get_data(bg_map *map, bg_string *key) {
  size_t i;
  get_data_index(map, key, &i);
  bg_string_free(key);

  if(!(i < map->data_pair_count)) {
    return NULL;
  }

  return map->data[i];
}

int bg_map_foreach(bg_map *map, int (*callback)(const bg_string*, void*, void*), void *output) {
  size_t i;
  int err;

  for(i = 0; i < map->data_pair_count; ++i) {
    if((err = callback(map->data_keys[i], map->data[i], output))) {
      return err;
    }
  }

  return 0;
}
