
#include "camera.h"
#include "particle.h"
#include "app.h"
#include "render_util.h"
#include "util/assert.h"
#include <easyx.h>


extern "C" {

void particle_Render(System_Particle *sys) {
	ASSERT(sys->super->camera && "particle_Render called without a Camera system");
	System_Camera *cam = sys->super->camera;

	for (tree_Node *i = tree_FirstNode(sys->parts);
		 i != NULL;
		 i = tree_Node_Next(i)) {
		Particle *p = (Particle *)i->data;

		render_SetModes(*p->mode, p->addtime);
		render_FillCircleW(sys->super, p->pos, p->size);
	}
}
}
