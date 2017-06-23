#ifndef _BLUR_H_
#define _BLUR_H_

#include <blurgather/context.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ERROR_AND_RETURN(ret, msg, ...) fprintf(stderr, msg, ##__VA_ARGS__); return ret


int blur_setup_context(bg_context *ctx,
                       bg_persister_t *persister,
                       bg_repository_t *repo,
                       bg_cryptor_t *cryptor);


bg_string *blur_getfield(const char *showing, int hide_input);

bg_secret_key_t *blur_ask_secret_key(bg_context *ctx);

size_t find_string_index(int argc, const char **argv, const char *str);


typedef int (* const blur_option)(bg_context *ctx, int argc, char **argv);
int run_options(bg_context *ctx, int argc, char **argv);


typedef int (* const blur_cmd)(bg_context *ctx, int argc, char **argv);
int run_command(bg_context *ctx, int argc, char **argv);


#ifdef __cplusplus
}
#endif

#endif
