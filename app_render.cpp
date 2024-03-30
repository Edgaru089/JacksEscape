
#include "app.h"
#include "camera.h"
#include "particle.h"
#include "physics.h"
#include "easyx.h"
#include "render_bundle.h"
#include "render_component.h"
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
	render_SetModes(render_ModeDefault, time_Now());

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
		if (e->misc) {
			if (e->misc->textbox) {
				setlinecolor(RGB(0, 0, 255));
				Box2 box = camera_TransformBox2(app->camera, box2_Offset(e->misc->textbox->trigger_box, e->position->position));
				rectangle(
					(int)round(box.lefttop.x),
					(int)round(box.lefttop.y),
					(int)round(box.lefttop.x + box.size.x),
					(int)round(box.lefttop.y + box.size.y));
			}
		}
	}

	static FillMode mode_rotate = {
		.rotate = Duration{.microseconds = 100 * 1000}};


	setfillcolor(RGB(255, 255, 255));
	setbkcolor(RGB(0, 0, 0));
	render_SetModes(mode_rotate, since);
	render_FillCircleW(app, vec2(200, 100), 20);


	// render_DrawBundleW(app, render_FindBundle("info_plate"), vec2(600, 550));
	render_DrawBundleW(app, render_FindBundle("info_plate_small_1"), vec2(250, 200));
	render_DrawBundleW(app, render_FindBundle("info_plate_small_2"), vec2(750, 200));

	// Draw entities
	for (tree_Node *i = tree_FirstNode(app->entity->entities);
		 i != NULL;
		 i = tree_Node_Next(i)) {
		Entity *e = (Entity *)i->data;
		if (e->render) {
			Component_Render *r = e->render;

			// Calculate global position as offset
			Vec2 pos = {.x = 0.0, .y = 0.0};
			if (e->position)
				pos = e->position->position;
			// Has bundle
			if (r->bundle)
				render_DrawBundleW(app, r->bundle, pos);
			// Has custom rendering
			if (r->custom)
				r->custom(app, e, camera_TransformVec2(app->camera, pos), r->custom_data);
		}
	}


	// Draw particles
	setfillcolor(RGB(255, 255, 255));
	setbkcolor(RGB(0, 0, 0));
	particle_Render(app->particle);


	settextcolor(RGB(255, 255, 255));
}
}
