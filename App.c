
#include "App.h"
#include "Entity.h"
#include "Input.h"
#include "Physics_Component.h"
#include "Player_Component.h"
#include <stdlib.h>


App *app_NewApp() {
	App *app = malloc(sizeof(App));

	app->input   = input_NewSystem(app);
	app->physics = physics_NewSystem(app);
	app->player  = player_NewSystem(app);
	app->entity  = entity_NewSystem(app);

	app->wantQuit = false;

	return app;
}

void app_DeleteApp(App *app) {
	input_DeleteSystem(app->input);
	entity_DeleteSystem(app->entity);
	physics_DeleteSystem(app->physics);
	player_DeleteSystem(app->player);

	free(app);
}


void app_Advance(App *app, Duration deltaTime) {
	input_Advance(app->input);
	player_Advance(app->player, deltaTime);
	physics_Advance(app->physics, deltaTime);
	entity_Advance(app->entity, deltaTime);
}
