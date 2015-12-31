#include <unistd.h>
#include <assert.h>

#include "password.h"
#include "password_factory.h"
#include "mcrypt_cryptor.h"
#include "password_msgpack_persister.h"
#include <kiki/utilities.h>
#include "mcrypt_iv.h"
#include "utils.h"
#include "clipboard.h"


int unlock_pwd_mng(bg_secret_key* secret_key, int confirmation_needed) {
	init_char_array(password1, BLURGATHER_PWD_MAX_VALUE_LEN);
	init_char_array(password2, BLURGATHER_PWD_MAX_VALUE_LEN);

	char* password = getpass("master password: ");
	strcat(password1, password);
	if(confirmation_needed) {
		password = getpass("master password confirmation: ");
		strcat(password2, password);
		if(strcmp(password1, password2)) { return -1; }
	}

	return secret_key->update(secret_key, lened_str(password));
}

void get_field(char* out, char* showing, int max_length) {
	printf("%s", showing);
	fgets(out, max_length, stdin);
	fflush(stdin);
	int i;
	for(i = max_length - 1; i > -1; --i) {
		if(out[i] == '\n') {
			out[i] = 0;
			break;
		}
	}
}

int create_password_from_user(bg_password_factory* password_factory) {
	bg_password* password = password_factory->new_password(password_factory);

	init_char_array(name, BLURGATHER_PWD_MAX_NAME_LEN);
	get_field(name, "password name: ", BLURGATHER_PWD_MAX_NAME_LEN);

	init_char_array(description, BLURGATHER_PWD_MAX_DESCRIPTION_LEN);
	get_field(description, "description: ", BLURGATHER_PWD_MAX_DESCRIPTION_LEN);

	strcat(password->name, name);
	strcat(password->description, description);

	init_char_array(value1, BLURGATHER_PWD_MAX_VALUE_LEN);
	init_char_array(value2, BLURGATHER_PWD_MAX_VALUE_LEN);
	char* value = getpass("password value: ");
	strcat(value1, value);
	value = getpass("password value confirmation: ");
	strcat(value2, value);
	if(strcmp(value1, value2)) {
		fprintf(stderr, "password values do not match!\n");
		return -1;
	}

	password->update(password, lened_str(value1));

	int return_value = 0;
	if((return_value = password->crypt(password))) { return return_value; }
	if((return_value = password->save(password))) { return return_value; }

	return return_value;
}

int add_password(bg_password_factory* password_factory, bg_password_repository* repository, bg_secret_key* secret_key) {
	if(unlock_pwd_mng(secret_key, 1)) {
		fprintf(stderr, "master password values do not match!\n");
		return 1;
	}
	if(create_password_from_user(password_factory)) {
		return 1;
	}
	if(repository->persist(repository)) {
		fprintf(stderr, "could not persist new password!\n");
		return 1;
	}

	return 0;
}

int kiPassword_compare_names(void* attribute, bg_password* password) {
	return strcmp((const char*)attribute, password->name);
}

typedef int kiPassword_compare_callback_t(void* attribute, bg_password* password);

bg_password* find_password_by_attribute(bg_password_repository* repository, void* attribute, kiPassword_compare_callback_t compare_callback) {
	bg_password* result = NULL;
	bg_password_iterator iterator = repository->begin(repository);
	bg_password_iterator end = repository->end(repository);

	for(; iterator.value != end.value; iterator.next(&iterator)) {
		if(compare_callback(attribute, (*iterator.value)) == 0) {
			result = *iterator.value;
			break;
		}
	}

	return result;
}

int send_password_to_user(bg_password_repository* repository, const char* name, bg_secret_key* secret_key) {
	bg_password* password = find_password_by_attribute(repository, (void*)name, kiPassword_compare_names);

	if(!password) {
		fprintf(stderr, "no such password!\n");
		return 1;
	}
	if(unlock_pwd_mng(secret_key, 0)) {
		fprintf(stderr, "could not unlock master key!");
		return 1;
	}
	if(password->decrypt(password)) {
		fprintf(stderr, "could not decrypt password!");
		return 1;
	}
	send_to_clipboard(password->value);
	password->crypt(password);

	return 0;
}

char* get_persistance_filename() {
	char* persistance_file = calloc(500, sizeof(char));
	strcat(persistance_file, getenv("HOME"));
	strcat(persistance_file, "/.blrgthrrc");
	return persistance_file;
}

#define DESTROY_STACK_OBJECTS_AND_RETURN(_return_value_) \
repository.destroy(&repository);\
cryptor.destroy(&cryptor);\
return _return_value_;

int main(int argc, char** argv) {
	assert(argc > 1 && argc < 4);

	bg_password_factory password_factory;
	bg_mcrypt_cryptor cryptor;
	bg_password_msgpack_persister repository;

	bg_mcrypt_cryptor_init(&cryptor);

	char* persistance_filename = get_persistance_filename();
	bg_password_msgpack_persister_init(&repository, persistance_filename, &password_factory);
	free(persistance_filename);

	bg_password_factory_init(&password_factory, &bg_mcrypt_iv_init, &repository.repository, &cryptor.encryptor,
	                                    &cryptor.decryptor);

	int load_return_value = repository.repository.load(&repository.repository);
	if(load_return_value != 0 && load_return_value != -4) {
		fprintf(stderr, "could not load password repository");
		DESTROY_STACK_OBJECTS_AND_RETURN(1);
	}

	if(!strcmp(argv[1], "add") && argc == 2) {
		if(add_password(&password_factory, &repository.repository, cryptor.secret_key)) {
			DESTROY_STACK_OBJECTS_AND_RETURN(1);
		}
	} else if(!strcmp(argv[1], "info")) {
		printf("%zu passwords in repo\n", repository.number_passwords);
	} else if(!strcmp(argv[1], "get")) {
		if(argc != 3) {
			fprintf(stderr, "no name provided!");
			DESTROY_STACK_OBJECTS_AND_RETURN(1);
		}
		if(send_password_to_user(&repository.repository, argv[2], cryptor.secret_key)) {
			DESTROY_STACK_OBJECTS_AND_RETURN(1);
		}
	}

	DESTROY_STACK_OBJECTS_AND_RETURN(0);
}
