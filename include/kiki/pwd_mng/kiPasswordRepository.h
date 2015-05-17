#ifndef KIKI_PWD_MNG_KIPASSWORDREPOSITORY_H
#define KIKI_PWD_MNG_KIPASSWORDREPOSITORY_H

#include <uuid/uuid.h>

#ifdef __cplusplus
extern "C" {
#endif

declare_kiclass(kiPasswordRepository);

#include "kiPassword.h"
#include "kiPasswordIterator.h"

struct kiPasswordRepository {
	void* object;

	int (* add)(kiPasswordRepository* self, kiPassword* password);
	int (* remove)(kiPasswordRepository* self, uuid_t password_uuid);

	void (* sort)(kiPasswordRepository* self);
	kiPasswordIterator (* begin)(kiPasswordRepository* self);
	kiPasswordIterator (* end)(kiPasswordRepository* self);

	int (* load)(kiPasswordRepository* self);
	int (* persist)(kiPasswordRepository* self);
};

#ifdef __cplusplus
}
#endif

#endif //KIKI_PWD_MNG_KIPASSWORDREPOSITORY_H
