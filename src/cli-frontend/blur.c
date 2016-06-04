#include <unistd.h>
#include <assert.h>

#include "password.h"
#include "password_factory.h"
#include "mcrypt_cryptor.h"
#include "password_msgpack_persister.h"
#include "utilities.h"
#include "mcrypt_iv.h"
#include "utils.h"
#include "clipboard.h"


#define CONFIRMATION_NEEDED     1
#define CONFIRMATION_NOT_NEEDED 0

int unlock_pwd_mng(bg_secret_key* secret_key, int confirmation_needed);
void getfield(char* out, const char* showing, size_t max_length);

char *(*blur_getmasterpass)(const char *showing) = &getpass;
char *(*blur_getpass)(const char *showing) = &getpass;
void (*blur_getnamefield)(char *output, const char *showing, size_t max_lenght) = &getfield;
void (*blur_getdescriptionfield)(char *output, const char *showing, size_t max_lenght) = &getfield;

int unlock_pwd_mng(bg_secret_key* secret_key, int confirmation) {
	init_char_array(password1, BLURGATHER_PWD_MAX_VALUE_LEN);
	init_char_array(password2, BLURGATHER_PWD_MAX_VALUE_LEN);

	char* password = blur_getmasterpass("master password: ");
	strcat(password1, password);
	if(confirmation == CONFIRMATION_NEEDED) {
		password = blur_getmasterpass("master password confirmation: ");
		strcat(password2, password);
		if(strcmp(password1, password2)) { return -1; }
  }

	return secret_key->update(secret_key, lened_str(password));
}

void getfield(char* out, const char* showing, size_t max_length) {
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

char *fixed_master;
char *fixed_value;
char *fixed_name;
char *fixed_description;

char *getmasterpass_fixed(const char *showing) {
  return fixed_master;
}
void getnamefield_fixed(char* out, const char* showing, size_t max_length) {
  strcpy(out, fixed_name);
}
void getdescriptionfield_fixed(char* out, const char* showing, size_t max_length) {
  strcpy(out, fixed_description);
}
char *getpass_fixed(const char *showing) {
  return fixed_value;
}

int create_password_from_user(bg_password_factory* password_factory) {
	bg_password* password = password_factory->new_password(password_factory);

	init_char_array(name, BLURGATHER_PWD_MAX_NAME_LEN);
	blur_getnamefield(name, "password name: ", BLURGATHER_PWD_MAX_NAME_LEN);

	init_char_array(description, BLURGATHER_PWD_MAX_DESCRIPTION_LEN);
	blur_getdescriptionfield(description, "description: ", BLURGATHER_PWD_MAX_DESCRIPTION_LEN);

        bg_password_update_name(password, name);
	bg_password_update_description(password, description);

	init_char_array(value1, BLURGATHER_PWD_MAX_VALUE_LEN);
	init_char_array(value2, BLURGATHER_PWD_MAX_VALUE_LEN);
	char* value = blur_getpass("password value: ");
	strcat(value1, value);
	value = blur_getpass("password value confirmation: ");
	strcat(value2, value);
	if(strcmp(value1, value2)) {
		fprintf(stderr, "password values do not match!\n");
		return -1;
	}

	bg_password_update_value(password, value1);

	int return_value = 0;
	if((return_value = bg_password_crypt(password))) { return return_value; }
	if((return_value = bg_password_save(password))) { return return_value; }

	return return_value;
}

int add_password(bg_password_factory* password_factory, bg_password_repository* repository, bg_secret_key* secret_key) {
	if(unlock_pwd_mng(secret_key, CONFIRMATION_NEEDED)) {
		fprintf(stderr, "master password values do not match!\n");
		return 1;
	}
	if(create_password_from_user(password_factory)) {
		return 1;
	}
	if(bg_pwd_repository_persist(repository)) {
		fprintf(stderr, "could not persist new password!\n");
		return 1;
	}

	return 0;
}

int password_compare_names(void* attribute, bg_password* password) {
	return strcmp((const char*)attribute, bg_password_name(password));
}

typedef int password_compare_callback_t(void* attribute, bg_password* password);

bg_password* find_password_by_attribute(bg_password_repository* repository, void* attribute, password_compare_callback_t compare_callback) {
	bg_password* result = NULL;
	bg_password_iterator iterator = bg_pwd_repository_begin(repository);
	bg_password_iterator end = bg_pwd_repository_end(repository);

	for(; iterator.value != end.value; iterator.next(&iterator)) {
		if(compare_callback(attribute, (*iterator.value)) == 0) {
			result = *iterator.value;
			break;
		}
	}

	return result;
}

int print_pwd_name(bg_password *password, void *output_stream) {
  fprintf((FILE *)output_stream, "%s\n", bg_password_name(password));
  return 0;
}

int list_password_names(FILE *output_stream, bg_password_repository *repo, bg_secret_key *secret_key) {
  if(unlock_pwd_mng(secret_key, CONFIRMATION_NOT_NEEDED)) {
		fprintf(stderr, "could not unlock master key!");
		return 1;
	}

  return bg_pwd_repository_foreach(repo, &print_pwd_name, output_stream);
}

int send_password_to_user(bg_password_repository* repository, const char* name, bg_secret_key* secret_key) {
	bg_password* password = find_password_by_attribute(repository, (void*)name, password_compare_names);

	if(!password) {
		fprintf(stderr, "no such password!\n");
		return 1;
	}
	if(unlock_pwd_mng(secret_key, CONFIRMATION_NOT_NEEDED)) {
		fprintf(stderr, "could not unlock master key!");
		return 1;
	}
	if(bg_password_decrypt(password)) {
		fprintf(stderr, "could not decrypt password!");
		return 1;
	}
	send_to_clipboard(bg_password_value(password));
	bg_password_crypt(password);

	return 0;
}

const char * esc_dquotes(const char *str) {
  static char buffer[BLURGATHER_PWD_MAX_DESCRIPTION_LEN];

  memset(buffer, 0, sizeof(BLURGATHER_PWD_MAX_DESCRIPTION_LEN));

  int i = 0; size_t max = strlen(str);
  while(i < max) {
    if(str[i] == '"') {
      char esc[] =  {'\\', 0};
      strcat(buffer, esc);
    }
    char _char[] =  {str[i], 0};
    strcat(buffer, _char);
    ++i;
  }

  return buffer;
}

int dump_password(bg_password *pwd, void *file) {
  FILE *dumpfile = ((FILE*)file);

  if(bg_password_decrypt(pwd)) {
    return -1;
  }

  fprintf(dumpfile, "        {\n            \"name\" : \"%s\",\n", esc_dquotes(bg_password_name(pwd)));
  fprintf(dumpfile, "            \"description\" : \"%s\",\n", esc_dquotes(bg_password_description(pwd)));
  fprintf(dumpfile, "            \"value\" : \"%s\"\n        },\n", esc_dquotes(bg_password_value(pwd)));

  if(bg_password_crypt(pwd)) {
    return -1;
  }

  return 0;
}

int migrate_passwords_to_file(bg_password_repository *repo, const char *filename, bg_secret_key* secret_key) {
  if(unlock_pwd_mng(secret_key, CONFIRMATION_NEEDED)) {
		fprintf(stderr, "could not unlock master key!");
		return 1;
	}

  FILE* dumpfile = fopen(filename, "w");
  fprintf(dumpfile, "{\n    \"passwords\" : [\n");
  if(bg_pwd_repository_foreach(repo, &dump_password, dumpfile)) {
    fclose(dumpfile);
    return -1;
  }
  fseek(dumpfile, -2, SEEK_END);
  fprintf(dumpfile, "\n    ]\n}");
  fclose(dumpfile);

  return 0;
}

int remove_password(bg_password_repository *repo, const char *name, bg_secret_key *secret_key) {
  if(unlock_pwd_mng(secret_key, CONFIRMATION_NEEDED)) {
    fprintf(stderr, "could not unlock password manager!");
    return 1;
  }
  if(bg_pwd_repository_remove(repo, name)) {
    fprintf(stderr, "no such entry!");
    return -1;
  }
  if(bg_pwd_repository_persist(repo)) {
    fprintf(stderr, "could not persist");
    return -2;
  }

  return 0;
}

char* get_persistance_filename() {
	char* persistance_file = calloc(500, sizeof(char));
	strcat(persistance_file, getenv("HOME"));
	strcat(persistance_file, "/.blrgthrrc");
	return persistance_file;
}

#define DESTROY_STACK_OBJECTS_AND_RETURN(_return_value_) \
bg_pwd_repository_destroy(&repository.repository);\
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

	bg_password_factory_init(&password_factory, &bg_mcrypt_iv_init, &repository.repository, &cryptor.encryptor, &cryptor.decryptor);

	int load_return_value = bg_pwd_repository_load(&repository.repository);
	if(load_return_value != 0 && load_return_value != -4) {
		fprintf(stderr, "could not load password repository: %d", load_return_value);
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
	} else if(!strcmp(argv[1], "list")) {
		if(list_password_names(stdout, &repository.repository, cryptor.secret_key)) {
			DESTROY_STACK_OBJECTS_AND_RETURN(1);
		}
	} else if(!strcmp(argv[1], "migrate")) {
		if(argc != 3) {
			fprintf(stderr, "no filename provided!");
			DESTROY_STACK_OBJECTS_AND_RETURN(1);
		}
		if(migrate_passwords_to_file(&repository.repository, argv[2], cryptor.secret_key)) {
			DESTROY_STACK_OBJECTS_AND_RETURN(1);
		}
	} else if(!strcmp(argv[1], "remove")) {
    if(argc != 3) {
			fprintf(stderr, "no entry provided!");
			DESTROY_STACK_OBJECTS_AND_RETURN(1);
		}
    if(remove_password(&repository.repository, argv[2], cryptor.secret_key)) {
      DESTROY_STACK_OBJECTS_AND_RETURN(1);
    }
  }

	DESTROY_STACK_OBJECTS_AND_RETURN(0);
}
