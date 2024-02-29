#pragma once

#include "util/tree.h"
#include "util/vector.h"
#include "Types.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _Entity Entity;

extern double physics_Gravity;


typedef struct {
	Entity *super;
	Vec2    position;
	Vec2    velocity;
} Component_Position;


// Box is relative to Position if exists
// if not, Box is absolute
//
// Moving hitboxes only hits fixed hitboxes
typedef struct {
	Entity *super;
	Box2    box;
	bool    fixed;
} Component_Hitbox;

// Returns the absolute version of the hitbox.
Box2 physics_HitboxAbsolute(Component_Hitbox *hitbox);


// Physics manager
typedef struct {
	// Every Position & Hitbox components
	tree_Tree     *pos, *hit;
	vector_Vector *flaggedDelete;
} System_Physics;

// Returns an empty physics system.
System_Physics *physics_NewSystem();
// Frees a physics system.
void physics_DeleteSystem(System_Physics *sys);

// Adds an entity.
void physics_AddEntity(System_Physics *sys, Entity *e);
// Deletes an entity.
void physics_DeleteEntity(System_Physics *sys, uintptr_t id);

// Advance is called on every frame.
void physics_Advance(System_Physics *sys, Duration deltaTime);


#ifdef __cplusplus
}
#endif
