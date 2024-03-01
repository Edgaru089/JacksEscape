#pragma once

#include <stdint.h>
#include "util/vector.h"
#include "Physics_Component.h"
#include "Player_Component.h"


#ifdef __cplusplus
extern "C" {
#endif


// Entity.
typedef struct _Entity {
	uintptr_t id;
	char     *name;

	Component_Position *position;
	Component_Hitbox   *hitbox;
	Component_Player   *player;
} Entity;


#ifdef __cplusplus
}
#endif
