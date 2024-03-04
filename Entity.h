#pragma once

#include <stdint.h>
#include "Types.h"
#include "util/vector.h"
#include "util/tree.h"
#include "Physics_Component.h"
#include "Player_Component.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct _Entity Entity;
typedef void (*entity_Thinker)(Entity *e, Duration deltaTime); // The Thinker function type assigned to Entities


// Entity.
typedef struct _Entity {
	uintptr_t id;
	char     *name; // Allocated on the stack & copied

	Component_Position *position;
	Component_Hitbox   *hitbox;
	Component_Player   *player;

	entity_Thinker thinker;     // Called by System_Entity each frame if not NULL.
	void          *thinkerData; // Data managed by the Thinker, if exists.
} Entity;


typedef struct _App App;

// Entity manager.
typedef struct {
	App       *super;
	tree_Tree *entities; // Contains Entity objects on the nodes
	uintptr_t  maxID;

	vector_Vector *flaggedDelete;
} System_Entity;

System_Entity *entity_NewSystem(App *super);
void           entity_DeleteSystem(System_Entity *sys);

Entity *entity_Create(System_Entity *sys, const char *name);
void    entity_Delete(System_Entity *sys, uintptr_t id);

void entity_Advance(System_Entity *sys, Duration deltaTime);


#ifdef __cplusplus
}
#endif
