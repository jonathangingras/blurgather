#ifndef _BLUR_CMDS_H_
#define _BLUR_CMDS_H_

#include <blurgather/context.h>

#ifdef __cplusplus
extern "C" {
#endif

/* commands */
int blur_cmd_add(bg_context *ctx, int argc, char **argv);
int blur_cmd_get(bg_context *ctx, int argc, char **argv);
int blur_cmd_list(bg_context *ctx, int argc, char **argv);
int blur_cmd_info(bg_context *ctx, int argc, char **argv);
int blur_cmd_remove(bg_context *ctx, int argc, char **argv);

/* options */
int blur_unlock_from_stdin(bg_context *ctx, int argc, char **argv);
int blur_persistence_filepath(bg_context *ctx, int argc, char **argv);
int blur_value_to_stdout(bg_context *ctx, int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif
