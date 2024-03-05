
#include "App.h"
#include "Entity.h"
#include "Input.h"
#include "Physics_Component.h"
#include "Player_Component.h"
#include "Types.h"
#include <stdlib.h>
#include <stdio.h>


static void _app_onHitBy(Entity *me, Entity *other, Vec2 triedDelta, void *data) {
	fprintf(
		stderr,
		"[_app_onHit] Entity \"%s\" hit by \"%s\", delta: [%.2lf, %.2lf]\n",
		me->name,
		other->name,
		triedDelta.x,
		triedDelta.y);
}


App *app_NewApp() {
	App *app = malloc(sizeof(App));

	app->input   = input_NewSystem(app);
	app->physics = physics_NewSystem(app);
	app->player  = player_NewSystem(app);
	app->entity  = entity_NewSystem(app);

	app->wantQuit = false;


	Entity *player = entity_Create(app->entity, "player");
	ADD_COMPONENT(player, player, zero_malloc(sizeof(Component_Player)));
	ADD_COMPONENT(player, position, zero_malloc(sizeof(Component_Position)));
	player->position->position = vec2(500, 500);
	player->position->velocity = vec2(0, 0);
	ADD_COMPONENT(player, hitbox, zero_malloc(sizeof(Component_Hitbox)));
	player->hitbox->box.lefttop = vec2(-20, -80);
	player->hitbox->box.size    = vec2(40, 80);
	entity_Commit(app->entity, player);

	Entity *hit1 = entity_Create(app->entity, "hit1");
	ADD_COMPONENT(hit1, hitbox, zero_malloc(sizeof(Component_Hitbox)));
	hit1->hitbox->box.lefttop = vec2(200, 200);
	hit1->hitbox->box.size    = vec2(100, 400);
	hit1->hitbox->fixed       = true;
	hit1->hitbox->onHitBy     = &_app_onHitBy;
	entity_Commit(app->entity, hit1);

	Entity *hit2 = entity_Create(app->entity, "hit2");
	ADD_COMPONENT(hit2, hitbox, zero_malloc(sizeof(Component_Hitbox)));
	hit2->hitbox->box.lefttop = vec2(700, 200);
	hit2->hitbox->box.size    = vec2(100, 400);
	hit2->hitbox->fixed       = true;
	hit2->hitbox->onHitBy     = &_app_onHitBy;
	entity_Commit(app->entity, hit2);

	Entity *hit3 = entity_Create(app->entity, "hit3");
	ADD_COMPONENT(hit3, hitbox, zero_malloc(sizeof(Component_Hitbox)));
	hit3->hitbox->box.lefttop = vec2(100, 550);
	hit3->hitbox->box.size    = vec2(800, 30);
	hit3->hitbox->fixed       = true;
	hit3->hitbox->onHitBy     = &_app_onHitBy;
	entity_Commit(app->entity, hit3);

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
