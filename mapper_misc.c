
#include "mapper_misc.h"
#include "app.h"
#include "entity.h"
#include "physics.h"
#include "player.h"
#include "types.h"
#include "util/assert.h"
#include <stdio.h>


void misc_DeleteComponent(Component_Misc *misc) {
	if (misc) {
		if (misc->textbox) {
			if (misc->textbox->text)
				free(misc->textbox->text);
			free(misc->textbox);
		}
		if (misc->respawn)
			free(misc->respawn);
		if (misc->hazard)
			free(misc->hazard);
		free(misc);
	}
}


void misc_thinker_HazardRespawn(App *app, Entity *e, Duration deltaTime) {
	if (!e->misc || !e->misc->respawn) {
		WARN("called on an entity without misc or misc.respawn", 0);
		e->thinker = NULL;
		return;
	}
	if (app->player->player == NULL) // No player
		return;
	Component_Player *p         = app->player->player;
	Box2              playerbox = physics_HitboxAbsolute(p->super->hitbox);

	Box2 worldbox; // Offset if position component exists
	Vec2 worldspawn;
	if (e->position) {
		worldbox   = box2_Offset(e->misc->respawn->trigger_box, e->position->position);
		worldspawn = vec2_Add(e->misc->respawn->respawn_pos, e->position->position);
	} else {
		worldbox   = e->misc->respawn->trigger_box;
		worldspawn = e->misc->respawn->respawn_pos;
	}

	if (box2_Intersects(worldbox, playerbox, NULL))
		p->hazardRespawn = worldspawn;
}

void misc_thinker_Hazard(App *app, Entity *e, Duration deltaTime) {
	if (!e->misc || !e->misc->hazard) {
		WARN("called on an entity without misc or misc.textbox", 0);
		e->thinker = NULL;
		return;
	}
	if (app->player->player == NULL) // No player
		return;
	Component_Player *p         = app->player->player;
	Box2              playerbox = physics_HitboxAbsolute(p->super->hitbox);

	Box2 worldbox; // Offset if position component exists
	if (e->position)
		worldbox = box2_Offset(*e->misc->hazard, e->position->position);
	else
		worldbox = *e->misc->hazard;

	if (box2_Intersects(worldbox, playerbox, NULL))
		player_HazardHarm(app->player);
}

void misc_thinker_Textbox(App *app, Entity *e, Duration deltaTime) {
	if (!e->misc || !e->misc->textbox) {
		WARN("called on an entity without misc or misc.textbox", 0);
		e->thinker = NULL;
		return;
	}
	misc_Textbox *t = e->misc->textbox;
	if (app->player->player == NULL) { // No player, fade out
		t->progress = fmaxf(0.0f, t->progress - duration_Seconds(deltaTime) * MISC_TEXTBOX_FADEIN_SPEED);
		return;
	}
	Component_Player *p         = app->player->player;
	Box2              playerbox = physics_HitboxAbsolute(p->super->hitbox);

	Box2 worldbox; // Offset if position component exists
	ASSERT(e->position && "Textboxes must have a position component");
	worldbox = box2_Offset(t->trigger_box, e->position->position);

	if (box2_Intersects(worldbox, playerbox, NULL))
		// Fade in
		t->progress = fminf(1.0f, t->progress + duration_Seconds(deltaTime) * MISC_TEXTBOX_FADEIN_SPEED);
	else
		// Fade out
		t->progress = fmaxf(0.0f, t->progress - duration_Seconds(deltaTime) * MISC_TEXTBOX_FADEIN_SPEED);
}

void misc_thinker_ToLive(App *app, Entity *e, Duration deltaTime) {
	if (!e->misc || e->misc->tolive.microseconds == 0) {
		WARN("called on an entity without misc or misc.tolive", 0);
		e->thinker = NULL;
		return;
	}

	if (e->misc->tolive.microseconds < time_Now().microseconds) {
		// After its allocated time
		entity_Delete(app->entity, e->id);
	}
}


// Utility functions for creating misc entities
void misc_InstantiateTextbox(App *app, Entity *e, const char *text, Box2 trigger_box, float offset) {
	ASSERT(e->misc == NULL && "Instantiate must be called with e.misc not set");
	ASSERT((e->render == NULL || e->render->custom == NULL) && "Instantiate for Textbox must be called with e.render.custom not set");
	e->misc          = zero_malloc(sizeof(Component_Misc));
	e->misc->textbox = zero_malloc(sizeof(misc_Textbox));

	e->misc->textbox->text        = copy_malloc(text);
	e->misc->textbox->trigger_box = trigger_box;
	e->misc->textbox->offset      = offset;

	if (!e->render)
		e->render = render_NewComponentFunc(app, &misc_render_Textbox, NULL);
	else
		e->render->custom = &misc_render_Textbox;
	e->thinker = &misc_thinker_Textbox;
}

void misc_InstantiateHazardRespawn(App *app, Entity *e, Box2 trigger_box, Vec2 respawn_pos) {
	ASSERT(e->misc == NULL && "Instantiate must be called with e.misc not set");
	e->misc          = zero_malloc(sizeof(Component_Misc));
	e->misc->respawn = zero_malloc(sizeof(misc_HazardRespawn));

	e->misc->respawn->respawn_pos = respawn_pos;
	e->misc->respawn->trigger_box = trigger_box;

	e->thinker = &misc_thinker_HazardRespawn;
}

void misc_InstantiateHazard(App *app, Entity *e, Box2 trigger_box) {
	ASSERT(e->misc == NULL && "Instantiate must be called with e.misc not set");
	e->misc         = zero_malloc(sizeof(Component_Misc));
	e->misc->hazard = copy_malloc_size(&trigger_box, sizeof(Box2));

	e->thinker = &misc_thinker_Hazard;
}

void misc_InstantiateToLive(App *app, Entity *e, Duration duration, TimePoint since) {
	ASSERT(e->misc == NULL && "Instantiate must be called with e.misc not set");
	e->misc         = zero_malloc(sizeof(Component_Misc));
	e->misc->tolive = time_After(since, duration);

	e->thinker = &misc_thinker_ToLive;
}
