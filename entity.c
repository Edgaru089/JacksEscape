
#include "entity.h"
#include "app.h"
#include "camera.h"
#include "mapper_misc.h"
#include "physics.h"
#include "render_component.h"
#include "util/assert.h"
#include "util/tree.h"
#include "util/vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// Frees every member if they exists.
static void _entity_FreeMembers(Entity *e) {
	if (e->position)
		free(e->position);
	if (e->hitbox)
		free(e->hitbox);
	if (e->player)
		free(e->player);
	if (e->name)
		free(e->name);
	if (e->render)
		render_DeleteComponent(e->render);
	if (e->misc)
		misc_DeleteComponent(e->misc);
}


System_Entity *entity_NewSystem(App *super) {
	System_Entity *sys = malloc(sizeof(System_Entity));

	sys->entities      = tree_Create(sizeof(Entity));
	sys->maxID         = 0;
	sys->super         = super;
	sys->flaggedDelete = vector_Create(sizeof(uintptr_t));

	return sys;
}

void entity_DeleteSystem(System_Entity *sys) {
	for (tree_Node *i = tree_FirstNode(sys->entities);
		 i != NULL;
		 i = tree_Node_Next(i)) {
		Entity *e = (Entity *)(i->data);

		physics_DeleteEntity(sys->super->physics, e->id);
		player_DeleteEntity(sys->super->player, e->id);
		camera_DeleteEntity(sys->super->camera, e->id);

		_entity_FreeMembers(e);
	}

	tree_Destroy(sys->entities);
	vector_Destroy(sys->flaggedDelete);
}


Entity *entity_Create(System_Entity *sys, const char *name) {
	uintptr_t id = ++sys->maxID;

	bool    added = false;
	Entity *e     = (Entity *)(tree_Insert(sys->entities, id, &added));
	ASSERT(added && "entity ID conflict in entity_Create");

	memset(e, 0, sizeof(Entity));
	e->id = id;

	size_t len;
	if (name && (len = strlen(name)) > 0) {
		e->name = malloc(len + 1);
		memcpy(e->name, name, len + 1);
	}

	return e;
}

void entity_Commit(System_Entity *sys, Entity *e) {
	physics_AddEntity(sys->super->physics, e);
	player_AddEntity(sys->super->player, e);
	camera_AddEntity(sys->super->camera, e);
}


static inline void _entity_Delete(System_Entity *sys, uintptr_t id) {
	tree_Node *node = tree_FindNode(sys->entities, id);
	if (!node) {
		fprintf(stderr, "[entity_Delete][WARN] Missing entity ID [%d]\n", id);
		return;
	}
	Entity *e = (Entity *)(node->data);

	fprintf(stderr, "[entity_Delete] Deleting \"%s\"[%d]\n", e->name, e->id);

	physics_DeleteEntity(sys->super->physics, e->id);
	player_DeleteEntity(sys->super->player, e->id);
	camera_DeleteEntity(sys->super->camera, e->id);


	_entity_FreeMembers(e);

	tree_Delete(sys->entities, node);
}

void entity_Delete(System_Entity *sys, uintptr_t id) {
	vector_Push(sys->flaggedDelete, &id);
}


void entity_Advance(System_Entity *sys, Duration deltaTime) {
	// Delete flagged entities
	for (int i = 0; i < vector_Size(sys->flaggedDelete); i++) {
		_entity_Delete(sys, *(uintptr_t *)(vector_At(sys->flaggedDelete, i)));
	}

	for (tree_Node *i = tree_FirstNode(sys->entities);
		 i != NULL;
		 i = tree_Node_Next(i)) {
		Entity *e = (Entity *)(i->data);

		if (e->thinker)
			e->thinker(sys->super, e, deltaTime);
	}
}


void entity_Clear(System_Entity *sys) {
	// Build a list of all entity IDs
	vector_Clear(sys->flaggedDelete);
	for (tree_Node *i = tree_FirstNode(sys->entities);
		 i != NULL;
		 i = tree_Node_Next(i)) {
		Entity *e = (Entity *)i->data;
		vector_Push(sys->flaggedDelete, &e->id);
	}

	// _Delete all of them
	for (int i = 0; i < vector_Size(sys->flaggedDelete); i++) {
		_entity_Delete(sys, *(uintptr_t *)(vector_At(sys->flaggedDelete, i)));
	}
	vector_Clear(sys->flaggedDelete);

	ASSERT(tree_Count(sys->entities) == 0);
}
