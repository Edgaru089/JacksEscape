
#include "app.h"
#include "camera.h"
#include "entity.h"
#include "input.h"
#include "particle.h"
#include "physics.h"
#include "player.h"
#include "types.h"
#include "render_bundle.h"
#include "render_component.h"
#include "mapper_misc.h"
#include <stdlib.h>
#include <stdio.h>


App *app_NewApp() {
	render_LoadBundle("bundles.txt");

	App *app = malloc(sizeof(App));

	app->input    = input_NewSystem(app);
	app->physics  = physics_NewSystem(app);
	app->player   = player_NewSystem(app);
	app->entity   = entity_NewSystem(app);
	app->camera   = camera_NewSystem(app);
	app->particle = particle_NewSystem(app);

	app->wantQuit = false;


	Entity *player = entity_Create(app->entity, "player");
	ADD_COMPONENT(player, player);
	ADD_COMPONENT(player, position);
	player->position->position = vec2(500, 500);
	player->position->velocity = vec2(0, 0);
	ADD_COMPONENT(player, hitbox);
	player->hitbox->box.lefttop = vec2(-20, -80);
	player->hitbox->box.size    = vec2(40, 80);
	entity_Commit(app->entity, player);

	Entity *hit1 = entity_Create(app->entity, "hit1");
	ADD_COMPONENT(hit1, hitbox);
	hit1->hitbox->box.lefttop = vec2(200, 200);
	hit1->hitbox->box.size    = vec2(100, 400);
	hit1->hitbox->fixed       = true;
	entity_Commit(app->entity, hit1);

	Entity *hit2 = entity_Create(app->entity, "hit2");
	ADD_COMPONENT(hit2, hitbox);
	hit2->hitbox->box.lefttop = vec2(700, 200);
	hit2->hitbox->box.size    = vec2(100, 400);
	hit2->hitbox->fixed       = true;
	entity_Commit(app->entity, hit2);

	Entity *hit3 = entity_Create(app->entity, "hit3");
	ADD_COMPONENT(hit3, hitbox);
	hit3->hitbox->box.lefttop = vec2(100, 550);
	hit3->hitbox->box.size    = vec2(800, 30);
	hit3->hitbox->fixed       = true;
	entity_Commit(app->entity, hit3);

	Entity *text1 = entity_Create(app->entity, "text_plate_large_1");
	ADD_COMPONENT(text1, position);
	text1->position->position = vec2(600, 550);
	text1->render             = render_NewComponent(app, "info_plate");
	misc_InstantiateTextbox(app, text1, "So long lives this,\nAnd this gives life to thee.", misc_TextboxUpright(110, 110), -220);
	entity_Commit(app->entity, text1);

	Entity *text2 = entity_Create(app->entity, "text_plate_1");
	ADD_COMPONENT(text2, position);
	text2->position->position = vec2(250, 200);
	text2->render             = render_NewComponent(app, "info_plate_small_1");
	misc_InstantiateTextbox(app, text2, "Press Space to jump.\nYou can jump again midair.", misc_TextboxUpright(70, 70), -180);
	entity_Commit(app->entity, text2);

	Entity *text3 = entity_Create(app->entity, "text_plate_2");
	ADD_COMPONENT(text3, position);
	text3->position->position = vec2(750, 200);
	text3->render             = render_NewComponent(app, "info_plate_small_2");
	misc_InstantiateTextbox(app, text3, "Press ; to dash.\nYou can only dash one time midair.", misc_TextboxUpright(70, 70), -180);
	entity_Commit(app->entity, text3);

	Entity *respawn1 = entity_Create(app->entity, "respawn1");
	misc_InstantiateHazardRespawn(app, respawn1, box2(300, 500, 400, 50), vec2(500, 550));
	entity_Commit(app->entity, respawn1);

	Entity *hazard1 = entity_Create(app->entity, "hazard1");
	misc_InstantiateHazard(app, hazard1, box2(800, 545, 100, 5));
	entity_Commit(app->entity, hazard1);

	return app;
}

void app_DeleteApp(App *app) {
	input_DeleteSystem(app->input);
	entity_DeleteSystem(app->entity);
	physics_DeleteSystem(app->physics);
	player_DeleteSystem(app->player);
	camera_DeleteSystem(app->camera);

	free(app);
}


void app_Advance(App *app, Duration deltaTime) {
	particle_Advance(app->particle, deltaTime);
	input_Advance(app->input);
	player_Advance(app->player, deltaTime);
	physics_Advance(app->physics, deltaTime);
	entity_Advance(app->entity, deltaTime);
	camera_Advance(app->camera, deltaTime);
}
