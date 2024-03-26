
#include "particle.h"
#include "types.h"
#include "util/tree.h"
#include "util/vector.h"
#include <stdio.h>


System_Particle *particle_NewSystem(App *super) {
	System_Particle *sys = zero_malloc(sizeof(System_Particle));

	sys->super       = super;
	sys->parts       = tree_Create(sizeof(Particle));
	sys->deleted_ids = vector_Create(sizeof(uintptr_t));
	sys->maxID       = 0;
	return sys;
}
void particle_DeleteSystem(System_Particle *sys) {
	tree_Destroy(sys->parts);
	vector_Destroy(sys->deleted_ids);
	free(sys);
}


static void _particle_AdvancePart(System_Particle *sys, Particle *p, Duration deltaTime) {
	p->size -= p->sizedec * duration_Seconds(deltaTime);
	if ((p->tolive.microseconds != 0 &&
		 time_Since(p->addtime).microseconds > p->tolive.microseconds) ||
		p->size < EPS) { // vanished
		particle_Delete(sys, p->id);
		return;
	}

	// Move
	Vec2 delta = vec2_Scale(p->vec, duration_Seconds(deltaTime));
	p->pos     = vec2_Add(p->pos, delta);
	// Move slower
	Vec2 delta_vec = vec2_Scale(p->vec, -p->vec_friction * duration_Seconds(deltaTime));
	p->vec         = vec2_Add(p->vec, delta_vec);
}

void particle_Advance(System_Particle *sys, Duration deltaTime) {
	// Clear particles marked as deleted
	for (int i = 0; i < vector_Size(sys->deleted_ids); i++) {
		uintptr_t  id = *(uintptr_t *)vector_At(sys->deleted_ids, i);
		tree_Node *n  = tree_FindNode(sys->parts, id);
		if (n != NULL)
			tree_Delete(sys->parts, n);
		else
			fprintf(stderr, "[particle_Delete][WARN] Missing particle ID [%d]\n", id);
	}
	vector_Clear(sys->deleted_ids);

	for (tree_Node *i = tree_FirstNode(sys->parts);
		 i != NULL;
		 i = tree_Node_Next(i)) {
		Particle *p = (Particle *)i->data;
		_particle_AdvancePart(sys, p, deltaTime);
	}
}

void particle_Delete(System_Particle *sys, uintptr_t id) {
	vector_Push(sys->deleted_ids, &id);
}


void particle_Emit(System_Particle *sys, Vec2 pos, Vec2 vec, double vec_friction, double size, double sizedec, Duration tolive, const FillMode *fill) {
	uintptr_t id = ++sys->maxID;
	Particle *p  = tree_Insert(sys->parts, id, NULL);
	memset(p, 0, sizeof(Particle));
	p->id           = id;
	p->addtime      = time_Now();
	p->tolive       = tolive;
	p->pos          = pos;
	p->vec          = vec;
	p->vec_friction = vec_friction;
	p->size         = size;
	p->sizedec      = sizedec;
	p->mode         = fill;
}
