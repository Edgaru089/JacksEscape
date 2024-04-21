
#include "particle.h"
#include "types.h"
#include "util/tree.h"
#include "util/vector.h"
#include "util/assert.h"
#include "app.h"
#include <stdio.h>


typedef struct {
	uint64_t layer;
	uint64_t id;
} _particle_DeleteRecord;

static inline void particle_Delete(System_Particle *sys, int layer, uint64_t id) {
	uint64_t a[2] = {layer, id};
	vector_Push(sys->deleted_ids, a);
}

System_Particle *particle_NewSystem(App *super) {
	System_Particle *sys = zero_malloc(sizeof(System_Particle));

	sys->super       = super;
	sys->deleted_ids = vector_Create(sizeof(_particle_DeleteRecord));
	sys->maxID       = 0;
	for (int i = 0; i < PARTICLE_LAYER_COUNT; i++)
		sys->parts[i] = tree_Create(sizeof(Particle));
	return sys;
}
void particle_DeleteSystem(System_Particle *sys) {
	for (int i = 0; i < PARTICLE_LAYER_COUNT; i++)
		tree_Destroy(sys->parts[i]);
	vector_Destroy(sys->deleted_ids);
	free(sys);
}


static void _particle_AdvancePart(System_Particle *sys, int layer, Particle *p, Duration deltaTime) {
	p->size -= p->sizedec * duration_Seconds(deltaTime);
	if ((p->tolive.microseconds != 0 &&
		 gametime_Since(sys->super->time, p->addtime).microseconds > p->tolive.microseconds) ||
		p->size < EPS) { // vanished
		particle_Delete(sys, layer, p->id);
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
	// Delete particles
	int len = vector_Size(sys->deleted_ids);
	for (int i = 0; i < len; i++) {
		_particle_DeleteRecord rec  = *(_particle_DeleteRecord *)vector_At(sys->deleted_ids, i);
		tree_Node             *node = tree_FindNode(sys->parts[rec.layer], rec.id);
		if (!node)
			WARN("particle id %llu in layer %llu not found", rec.id, rec.layer);
		else
			tree_Delete(sys->parts[rec.layer], node);
	}
	vector_Clear(sys->deleted_ids);

	for (int layer = 0; layer < PARTICLE_LAYER_COUNT; layer++) {
		for (tree_Node *i = tree_FirstNode(sys->parts[layer]);
			 i != NULL;
			 i = tree_Node_Next(i)) {
			Particle *p = (Particle *)i->data;
			_particle_AdvancePart(sys, layer, p, deltaTime);
		}
	}
}


void particle_Emit(System_Particle *sys, int layer, Vec2 pos, Vec2 vec, double vec_friction, double size, double sizedec, Duration tolive, const FillMode *fill) {
	ASSERT(layer >= 0 && layer <= (PARTICLE_LAYER_COUNT - 1));
	uintptr_t id = ++sys->maxID;
	Particle *p  = tree_Insert(sys->parts[layer], id, NULL);
	memset(p, 0, sizeof(Particle));
	p->id           = id;
	p->addtime      = gametime_Now(sys->super->time);
	p->tolive       = tolive;
	p->pos          = pos;
	p->vec          = vec;
	p->vec_friction = vec_friction;
	p->size         = size;
	p->sizedec      = sizedec;
	p->mode         = fill;
}


void particle_Clear(System_Particle *sys) {
	vector_Clear(sys->deleted_ids);
	for (int i = 0; i < PARTICLE_LAYER_COUNT; i++) {
		// Delete every first node
		int count = tree_Count(sys->parts[i]);
		while (count--)
			tree_Delete(sys->parts[i], tree_FirstNode(sys->parts[i]));
		ASSERT(tree_Count(sys->parts[i]) == 0);
	}
}
