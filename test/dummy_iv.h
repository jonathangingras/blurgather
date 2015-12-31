#ifndef KIKI_PWD_MNG_DUMMY_IV_H
#define KIKI_PWD_MNG_DUMMY_IV_H

#define DUMMY_IV_LENGTH 24

#ifdef __cplusplus
extern "C" {
#endif

int randomizeCalled;

int dummy_iv_randomize(IV_t* iv) {
	randomizeCalled = 1;
        return 0;
}

void dummy_iv_destroy(IV_t* iv) {
	free(iv->value);
}

IV_t* dummy_iv_init(IV_t* iv) {
	iv->randomize = &dummy_iv_randomize;
	iv->destroy = &dummy_iv_destroy;
	iv->value = (unsigned char*)calloc(DUMMY_IV_LENGTH, sizeof(char));
	iv->length = DUMMY_IV_LENGTH;
        return iv;
}

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_DUMMY_IV_H
