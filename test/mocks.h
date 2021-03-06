#include <stdio.h>
#include <blurgather/context.h>
#include <blurgather/cryptor.h>
#include <blurgather/iv.h>
#include <blurgather/repository.h>
#include <blurgather/persister.h>

/* global test variables */
extern bg_context *ctx;
extern FILE *logstream;

/* cryptor */
extern bg_cryptor_t mock_cryptor;
extern int mock_encrypt_called;
extern int mock_decrypt_called;
extern int mock_encrypt_return_value;
extern int mock_decrypt_return_value;
extern int mock_cryptor_generate_iv_called;
extern int mock_encrypted_length_called;

/* iv */
extern bg_iv_t *mock_iv;
extern char mock_iv_data[32];

/* secret key */
extern bg_secret_key_t *mock_secret_key;

/* repository */
extern struct bg_repository_vtable mock_repository_vtable;
extern bg_repository_t mock_repository;
extern int mock_repository_destroy_called;
extern int mock_repository_add_called;
extern int mock_repository_get_called;
extern int mock_repository_remove_called;
extern int mock_repository_count_called;
extern int mock_repository_foreach_called;
extern size_t mock_repository_count_return_value;

/* persister */
extern struct bg_persister_vtable mock_persister_vtable;
extern bg_persister_t mock_persister;
extern int mock_persister_load_called;
extern int mock_persister_persist_called;

/* setup & teardown */
void turnoff_debug();
void reset_debug();
void reset_context();
void reset_mock_secret_key();
void reset_mock_iv();
void reset_mock_cryptor();
void reset_mock_repository();
