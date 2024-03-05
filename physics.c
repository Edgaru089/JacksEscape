
#include "physics.h"
#include "entity.h"
#include "types.h"
#include "util/tree.h"
#include "util/vector.h"
#include "util/assert.h"

#include <stdlib.h>
#include <string.h>

double physics_Gravity = 5.0;


Box2 physics_HitboxAbsolute(Component_Hitbox *hitbox) {
	Entity *super = hitbox->super;
	if (!super->position)
		return hitbox->box;
	else {
		Box2 box    = hitbox->box;
		box.lefttop = vec2_Add(box.lefttop, super->position->position);
		return box;
	}
}


System_Physics *physics_NewSystem(App *super) {
	System_Physics *sys = malloc(sizeof(System_Physics));
	sys->super          = super;

	sys->pos = tree_Create(sizeof(uintptr_t));
	sys->hit = tree_Create(sizeof(uintptr_t));

	return sys;
}

void physics_DeleteSystem(System_Physics *sys) {
	tree_Destroy(sys->pos);
	tree_Destroy(sys->hit);
}


void physics_AddEntity(System_Physics *sys, Entity *e) {
	if (e->position)
		memcpy(tree_Insert(sys->pos, e->id, NULL), &e->position, sizeof(uintptr_t));
	if (e->hitbox) {
		if (!e->hitbox->fixed)
			ASSERT(e->position && "Entity has non-Fixed Hitbox but no position");
		memcpy(tree_Insert(sys->hit, e->id, NULL), &e->hitbox, sizeof(uintptr_t));
	}
}

void physics_DeleteEntity(System_Physics *sys, uintptr_t id) {
	tree_Node *n;
	n = tree_FindNode(sys->pos, id);
	if (n)
		tree_Delete(sys->pos, n);
	n = tree_FindNode(sys->hit, id);
	if (n)
		tree_Delete(sys->hit, n);
}


// Defined in Physics_Move.c
void _physics_MoveX(System_Physics *sys, Entity *e, Duration deltaTime);
void _physics_MoveY(System_Physics *sys, Entity *e, Duration deltaTime);


static inline void _physics_AdvanceEntity(System_Physics *sys, Entity *e, Duration deltaTime) {
	ASSERT(e->position && "_physics_AdvanceEntity() called on entity with no position");

	// Short path
	if (!e->hitbox || e->hitbox->fixed) {
		e->position->position =
			vec2_Add(
				e->position->position,
				vec2_Scale(
					e->position->velocity,
					duration_Seconds(deltaTime)));
		return;
	}

	// Long path
	_physics_MoveX(sys, e, deltaTime);
	_physics_MoveY(sys, e, deltaTime);
}


static double gravity = 1000;

void physics_Advance(System_Physics *sys, Duration deltaTime) {
	for (tree_Node *i = tree_FirstNode(sys->pos);
		 i != NULL;
		 i = tree_Node_Next(i)) {
		Component_Position *pos = *((Component_Position **)i->data);

		if (pos->super->hitbox && !pos->super->hitbox->fixed)
			pos->velocity.y += gravity * duration_Seconds(deltaTime);

		_physics_AdvanceEntity(sys, pos->super, deltaTime);
	}
}
