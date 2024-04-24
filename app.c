
#include "app.h"
#include "camera.h"
#include "entity.h"
#include "gametime.h"
#include "input.h"
#include "particle.h"
#include "physics.h"
#include "player.h"
#include "types.h"
#include "render_bundle.h"
#include "render_component.h"
#include "mapper_misc.h"
#include "ui.h"
#include <stdlib.h>
#include <stdio.h>


App *app_NewApp() {
	render_LoadBundle("bundles.txt");

	App *app = zero_malloc(sizeof(App));

	app->input    = input_NewSystem(app);
	app->physics  = physics_NewSystem(app);
	app->player   = player_NewSystem(app);
	app->entity   = entity_NewSystem(app);
	app->camera   = camera_NewSystem(app);
	app->particle = particle_NewSystem(app);
	app->time     = gametime_NewSystem(app);
	app->ui       = ui_NewSystem(app);
	ui_RebuildUI(app->ui);
	ui_PushState(app->ui, ui_Running);

	app->switch_level = NULL;
	app->timescale    = 1.0;
	app->clear_color  = 0;
	app->wantQuit     = false;
	app->paused       = false;


	Entity *player = entity_Create(app->entity, "player");
	ADD_COMPONENT(player, player);
	ADD_COMPONENT(player, position);
	player->position->position = vec2(500, 500);
	player->position->velocity = vec2(0, 0);
	ADD_COMPONENT(player, hitbox);
	player->hitbox->box.lefttop = vec2(-20, -80);

	app_QueueLoadLevel(app, "intro.txt");
	_app_SwitchLevel(app);

	return app;
}

void app_DeleteApp(App *app) {
	input_DeleteSystem(app->input);
	entity_DeleteSystem(app->entity);
	physics_DeleteSystem(app->physics);
	player_DeleteSystem(app->player);
	camera_DeleteSystem(app->camera);
	particle_DeleteSystem(app->particle);
	gametime_DeleteSystem(app->time);
	ui_DeleteSystem(app->ui);

	free(app);
}


void app_Advance(App *app, Duration deltaTime) {
	// Limit deltaTime to 20ms (1/50 second)
	if (deltaTime.microseconds > 20000)
		deltaTime.microseconds = 20000;

	if (app->switch_level != NULL)
		_app_SwitchLevel(app);

	input_Advance(app->input);

	Duration delta_game = deltaTime;
	if (1.0 - app->timescale > EPS)
		delta_game.microseconds = delta_game.microseconds * app->timescale;

	if (!app->paused) {
		gametime_Advance(app->time, delta_game);
		particle_Advance(app->particle, delta_game);
		player_Advance(app->player, delta_game);
		physics_Advance(app->physics, delta_game);
		entity_Advance(app->entity, delta_game);
		camera_Advance(app->camera, delta_game);
	}

	ui_Advance(app->ui, deltaTime);
}
