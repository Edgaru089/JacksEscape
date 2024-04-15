
#include "app.h"
#include "camera.h"
#include "mapper_misc.h"
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
			if (e->misc->textbox)
				setlinecolor(RGB(0, 0, 255));
			if (e->misc->respawn_pos) {
				setlinecolor(RGB(255, 0, 255));
				Vec2 pos;
				if (e->position)
					pos = camera_TransformVec2(app->camera, vec2_Add(*e->misc->respawn_pos, e->position->position));
				else
					pos = camera_TransformVec2(app->camera, *e->misc->respawn_pos);
				line((int)round(pos.x + 12), (int)round(pos.y), (int)round(pos.x - 12), (int)round(pos.y));
				line((int)round(pos.x), (int)round(pos.y + 12), (int)round(pos.x), (int)round(pos.y - 12));
			}
			if (e->misc->trigger_flags & misc_Hazard)
				setlinecolor(RGB(255, 0, 0));
			if (e->misc->trigger_flags & misc_CameraFocus)
				setlinecolor(RGB(0, 255, 255));
			if (e->misc->change_level)
				setlinecolor(RGB(255, 255, 0));
			Box2 box;
			if (e->position)
				box = camera_TransformBox2(app->camera, box2_Offset(e->misc->trigger, e->position->position));
			else
				box = camera_TransformBox2(app->camera, e->misc->trigger);
			rectangle(
				(int)round(box.lefttop.x),
				(int)round(box.lefttop.y),
				(int)round(box.lefttop.x + box.size.x),
				(int)round(box.lefttop.y + box.size.y));
		}
	}
	if (app->player->player) {
		setlinecolor(RGB(255, 0, 255));
		Vec2 respawn = camera_TransformVec2(app->camera, app->player->player->hazardRespawn);
		circle((int)round(respawn.x), (int)round(respawn.y), 6);
	}


	setfillcolor(RGB(255, 255, 255));
	setbkcolor(RGB(0, 0, 0));
	render_SetModes(render_ModeRotate, since);
	render_FillCircleW(app, vec2(200, 100), 20);


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
			// Has fillbox
			if (r->fillbox.size.x > EPS) {
				Box2 cam = camera_TransformBox2(app->camera, r->fillbox);
				setfillcolor(r->fillcolor);
				fillrectangle(
					(int)round(cam.lefttop.x),
					(int)round(cam.lefttop.y),
					(int)round(cam.lefttop.x + cam.size.x),
					(int)round(cam.lefttop.y + cam.size.y));
			}
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
