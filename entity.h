#pragma once

#include <stdint.h>
#include "types.h"
#include "util/vector.h"
#include "util/tree.h"
#include "physics.h"
#include "player.h"
#include "render_component.h"
#include "mapper_misc.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct _App    App;
typedef struct _Entity Entity;
typedef void (*entity_Thinker)(App *app, Entity *e, Duration deltaTime); // The Thinker function type assigned to Entities


// Entity.
typedef struct _Entity {
	uintptr_t id;
	char     *name; // Allocated on the stack & copied

	Component_Position *position;
	Component_Hitbox   *hitbox;
	Component_Player   *player;
	Component_Render   *render;
	Component_Misc     *misc;

	entity_Thinker thinker;     // Called by System_Entity each frame if not NULL.
	void          *thinkerData; // Data managed by the Thinker, if exists.
} Entity;

// https://en.cppreference.com/w/c/language/sizeof
// The expression in sizeof is not evaluated
#define ADD_COMPONENT(entity, component)                                    \
	do {                                                                    \
		entity->component        = zero_malloc(sizeof(*entity->component)); \
		entity->component->super = entity;                                  \
	} while (false)


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
// After the Entity from Create is assigned its components,
// it should be commited into the system via this function.
void entity_Commit(System_Entity *sys, Entity *e);
void entity_Delete(System_Entity *sys, uintptr_t id);

void entity_Advance(System_Entity *sys, Duration deltaTime);


#ifdef __cplusplus
}
#endif
