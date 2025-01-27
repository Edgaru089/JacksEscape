
#include "app.h"
#include "camera.h"
#include "mapper_misc.h"
#include "particle.h"
#include "physics.h"
#include "easyx.h"
#include "render_bundle.h"
#include "render_component.h"
#include "ui.h"
#include "util/tree.h"
#include "types.h"
#include "render_util.h"
#include "util/vector.h"
#include <math.h>
#include <stdio.h>
#include <graphics.h>


namespace {
TimePoint since = time_Now();
} // namespace


extern "C" void app_Render(App *app) {
	render_SetModes(render_ModeDefault, time_Now());
	setbkcolor(app->clear_color);
	cleardevice();

	// Draw fill polys & boxes first
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

			// Has fill polygon
			if (r->fillpoly) {
				static vector_Vector *buf = vector_Create(2 * sizeof(int));
				vector_Clear(buf);

				// Transform every point in the polygon
				for (int i = 0; i < vector_Size(r->fillpoly); i++) {
					Vec2 screen_point        = camera_TransformVec2(app->camera, vec2_Add(*(Vec2 *)vector_At(r->fillpoly, i), pos));
					int  screen_point_int[2] = {
                        (int)round(screen_point.x),
                        (int)round(screen_point.y)};
					vector_Push(buf, screen_point_int);
				}

				// Draw
				setfillcolor(r->fillcolor);
				solidpolygon((POINT *)vector_Data(buf), vector_Size(r->fillpoly));
			}

			// Has fillbox
			if (r->fillbox.size.x > EPS) {
				Box2 cam = camera_TransformBox2(app->camera, box2_Offset(r->fillbox, pos));
				setfillcolor(r->fillcolor);
				solidrectangle(
					(int)round(cam.lefttop.x),
					(int)round(cam.lefttop.y),
					(int)round(cam.lefttop.x + cam.size.x),
					(int)round(cam.lefttop.y + cam.size.y));
			}
		}
	}


	// Debug boxes
	if (app->debugOn) {
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
	}


	setfillcolor(RGB(255, 255, 255));
	setbkcolor(RGB(0, 0, 0));

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

	// Draw player
	if (app->player->player) {
		setfillcolor(0xffffff);
		setlinecolor(0);
		Vec2 screen = camera_TransformVec2(app->camera, vec2_Add(box2_Center(app->player->player->super->hitbox->box), app->player->player->super->position->position));
		fillcircle((int)round(screen.x), (int)round(screen.y), (int)round(app->player->player->super->hitbox->box.size.y / 2.0));
	}


	// Draw particles
	setfillcolor(RGB(255, 255, 255));
	setbkcolor(RGB(0, 0, 0));
	particle_Render(app->particle);


	settextcolor(RGB(255, 255, 255));
	setbkcolor(RGB(0, 0, 0));

	// If paused, display a text
	if (app->paused)
		render_DrawTextEx("Game Paused", box2(SCREEN_WIDTH / 2.0 - 10, 100, 20, 100), DT_CENTER | DT_NOCLIP);
	if (1.0 - app->timescale > EPS) {
		char buf[128];
		snprintf(buf, sizeof(buf), "*** TIMESCALE %.2lf ***", app->timescale);
		render_DrawTextEx(buf, box2(SCREEN_WIDTH / 2.0 - 10, 50, 20, 100), DT_CENTER | DT_NOCLIP);
	}

	// Display a Timer
	if (strcmp(app->current_level, "title.txt") != 0) {
		char buf[64];
		snprintf(
			buf, sizeof(buf),
			"%02d:%02d.%02d",
			app->level_playtime.microseconds / 1000 / 1000 / 60,
			app->level_playtime.microseconds / 1000 / 1000 % 60,
			app->level_playtime.microseconds / 10000 % 100);
		render_DrawTextEx(buf, box2(0, 0, SCREEN_WIDTH, TEXTHEIGHT), DT_RIGHT);
	}

	// Draw UI
	ui_Render(app->ui);
}
