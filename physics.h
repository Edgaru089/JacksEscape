#pragma once

#include "util/tree.h"
#include "util/vector.h"
#include "types.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _Entity Entity;

extern double physics_Gravity;


// Position component.
//
// If not exist, it's like it's at (0,0)
typedef struct {
	Entity *super;
	Vec2    position;
	Vec2    velocity;
} Component_Position;


// Handler called when hitboxes are hit
typedef void (*physics_HitHandler)(Entity *me, Entity *other, Vec2 triedDelta, void *data);


// Box is relative to Position if exists
// if not, Box is absolute
//
// Moving hitboxes only hits fixed hitboxes
typedef struct {
	Entity *super;
	Box2    box;
	bool    fixed;

	physics_HitHandler onHit, onHitBy;
	void              *onHitData;
} Component_Hitbox;

// Returns the absolute version of the hitbox.
Box2 physics_HitboxAbsolute(Component_Hitbox *hitbox);


typedef struct _App App;

// Physics manager
typedef struct {
	App *super;
	// Every Position & Hitbox components
	tree_Tree *pos, *hit;
} System_Physics;

// Returns an empty physics system.
System_Physics *physics_NewSystem(App *super);
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
