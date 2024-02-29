
#include "Component_Physics.h"
#include "Entity.h"
#include "Types.h"
#include "util/tree.h"
#include "util/vector.h"
#include "util/assert.h"

#include <stdlib.h>
#include <string.h>

double physics_Gravity = 5.0;


System_Physics *physics_NewSystem() {
	System_Physics *sys = malloc(sizeof(System_Physics));

	sys->pos           = tree_Create(sizeof(uintptr_t));
	sys->hit           = tree_Create(sizeof(uintptr_t));
	sys->flaggedDelete = vector_Create(sizeof(uintptr_t));

	return sys;
}

void physics_DeleteSystem(System_Physics *sys) {
	tree_Destroy(sys->pos);
	tree_Destroy(sys->hit);
	vector_Destroy(sys->flaggedDelete);
}


void physics_AddEntity(System_Physics *sys, Entity *e) {
	if (e->position)
		memcpy(tree_Insert(sys->pos, e->id, NULL), &e->position, sizeof(uintptr_t));
	if (e->hitbox) {
		ASSERT(!e->hitbox->fixed && !e->position && "Entity has non-Fixed Hitbox but no position");
		memcpy(tree_Insert(sys->hit, e->id, NULL), &e->hitbox, sizeof(uintptr_t));
	}
}

void physics_DeleteEntity(System_Physics *sys, uintptr_t id) {
	vector_Push(sys->flaggedDelete, &id);
}


inline void _physics_AdvanceEntity(Entity *e, Duration deltaTime) {
	if (!e->position)
		return;

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
}


void physics_Advance(System_Physics *sys, Duration deltaTime) {
	// Delete flagged entities
	while (vector_Size(sys->flaggedDelete)) {
		uintptr_t id;
		vector_Pop(sys->flaggedDelete, &id);

		tree_Node *n;
		n = tree_FindNode(sys->pos, id);
		if (n)
			tree_Delete(sys->pos, n);
		n = tree_FindNode(sys->hit, id);
		if (n)
			tree_Delete(sys->hit, n);
	}

	for (tree_Node *i = tree_FirstNode(sys->pos);
		 i != NULL;
		 i = tree_Node_Next(i)) {
		Component_Position *pos = *((Component_Position **)i->data);
		_physics_AdvanceEntity(pos->super, deltaTime);
	}
}
