#ifndef PASSWORD_TO_MAP_H
#define PASSWORD_TO_MAP_H

#include "map.h"
#include "password.h"

#ifdef __cplusplus
extern "C" {
#endif

bg_map *bg_password_to_map(const bg_password *password);

#ifdef __cplusplus
}
#endif

#endif
