#ifndef KIKI_PWD_MNG_KIPASSWORDITERATOR_H
#define KIKI_PWD_MNG_KIPASSWORDITERATOR_H

#include <kiki/kiClass.h>

#include "kiPassword.h"

#ifdef __cplusplus
extern "C" {
#endif

declare_kiclass(kiPasswordIterator);

struct kiPasswordIterator {
	void* container;

	kiPassword** value;

	kiPassword** (* previous)(kiPasswordIterator* self);
	kiPassword** (* next)(kiPasswordIterator* self);
};

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_KIPASSWORDITERATOR_H
