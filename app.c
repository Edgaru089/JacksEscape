
#include "app.h"
#include "camera.h"
#include "entity.h"
#include "gametime.h"
#include "input.h"
#include "leaderboards.h"
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


#ifndef RGB
#define RGB(r, g, b) ((r) | ((g) << 8) | ((b) << 16))
#endif


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
	app->lboard   = lboard_NewSystem(app);
	ui_RebuildUI(app->ui);
	ui_PushState(app->ui, ui_TitleMenu);

	lboard_LoadFromFile(app->lboard, "leaderboards.txt");

	app->current_level = NULL;
	app->switch_level  = NULL;
	app->timescale     = 1.0;
	app->clear_color   = RGB(40, 40, 40);
	app->wantQuit      = false;
	app->paused        = false;

	app_QueueLoadLevel(app, "title.txt");

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
	lboard_DeleteSystem(app->lboard);

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
		app->level_playtime.microseconds += deltaTime.microseconds;
	}

	ui_Advance(app->ui, deltaTime);
}
