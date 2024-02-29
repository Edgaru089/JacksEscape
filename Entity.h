#pragma once

#include <stdint.h>
#include "util/vector.h"
#include "Component_Physics.h"


#ifdef __cplusplus
extern "C" {
#endif


// Entity.
typedef struct _Entity {
	uintptr_t   id;
	uintptr_t   type;
	const char *name;

	Component_Position *position;
	Component_Hitbox   *hitbox;

} Entity;


#ifdef __cplusplus
}
#endif
