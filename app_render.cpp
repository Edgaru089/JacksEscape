
#include "app.h"
#include "camera.h"
#include "particle.h"
#include "physics.h"
#include "easyx.h"
#include "util/tree.h"
#include "types.h"
#include "render_util.h"
#include <math.h>
#include <graphics.h>


namespace {
TimePoint since = time_Now();
} // namespace


extern "C" {


void app_Render(App *app) {
	for (tree_Node *i = tree_FirstNode(app->entity->entities);
		 i != NULL;
		 i = tree_Node_Next(i)) {
		Entity *e = (Entity *)(i->data);
		if (e->hitbox) {
			if (e->hitbox->fixed)
				setlinecolor(RGB(0, 255, 0));
			else
				setlinecolor(RGB(255, 255, 0));

			Box2 box = camera_TransformBox2(app->camera, physics_HitboxAbsolute(e->hitbox));
			rectangle(
				(int)round(box.lefttop.x),
				(int)round(box.lefttop.y),
				(int)round(box.lefttop.x + box.size.x),
				(int)round(box.lefttop.y + box.size.y));
		}
	}

	static FillMode mode_rotate = {
		.rotate = Duration{.microseconds = 100 * 1000}};


	setfillcolor(RGB(255, 255, 255));
	setbkcolor(RGB(0, 0, 0));
	render_SetModes(mode_rotate, since);
	render_FillCircleW(app, vec2(200, 100), 20);

	// Draw particles
	particle_Render(app->particle);
}
}
