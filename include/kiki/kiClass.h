#ifndef KIKI_KICLASS_H
#define KIKI_KICLASS_H

#ifdef __cplusplus
extern "C" {
#endif

#define declare_kiclass(x) \
	struct x;\
	typedef struct x x

#define kiclass(x) \
	struct x

struct kiobject {
	void (* destroy)(void* self);
};

#define ki_destroy(object_ptr) \
	((struct kiobject*)(object_ptr))->destroy((object_ptr))

#ifdef __cplusplus
}
#endif

#endif //KIKI_KICLASS_H
