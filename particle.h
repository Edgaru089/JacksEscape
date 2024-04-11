#pragma once

#include "types.h"
#include "render_util.h"
#include "util/tree.h"
#include "util/vector.h"


#ifdef __cplusplus
extern "C" {
#endif


// A particle instance.
typedef struct {
	uintptr_t id;            // ID.
	double    size, sizedec; // Diameter & decrease speed, size=0 and the particle is removed
	Duration  tolive;        // Time desiginated to live, 0 for forever
	TimePoint addtime;       // Time when this particle is added

	Vec2   pos, vec;     // Center & Speed
	double vec_friction; // Friction, fraction of Vec per second

	const FillMode *mode; // Fill Modes, NULL for default mode (solid fill)
} Particle;


typedef struct _App App;

// Particle system.
typedef struct {
	App           *super;
	tree_Tree     *parts; // uintptr_t -> struct Particle
	vector_Vector *deleted_ids;
	uintptr_t      maxID;
} System_Particle;

System_Particle *particle_NewSystem(App *super);
void             particle_DeleteSystem(System_Particle *sys);

void particle_Advance(System_Particle *sys, Duration deltaTime);
void particle_Render(System_Particle *sys);


// Emit a particle here.
void particle_Emit(System_Particle *sys, Vec2 pos, Vec2 vec, double vec_friction, double size, double sizedec, Duration tolive, const FillMode *fill);
void particle_Delete(System_Particle *sys, uintptr_t id);

// Clear all particles.
void particle_Clear(System_Particle *sys);


#ifdef __cplusplus
}
#endif
