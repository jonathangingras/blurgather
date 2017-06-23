#ifndef BLURGATHER_MAP_H
#define BLURGATHER_MAP_H

#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bg_map;
typedef struct bg_map bg_map;

bg_map *bg_map_new();
void bg_map_free(bg_map *map);

int bg_map_register_data(bg_map *map, bg_string *key, void *value);
void *bg_map_get_data(bg_map *map, bg_string *key);
int bg_map_foreach(bg_map *map, int (*callback)(const bg_string*, void*, void*), void *output);

#ifdef __cplusplus
}
#endif

#endif
