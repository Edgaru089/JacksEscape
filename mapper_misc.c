
#include "mapper_misc.h"
#include "app.h"
#include "entity.h"
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
		return;
	}
	if (app->player->player == NULL) // No player
		return;
	Component_Player *p = app->player->player;

	Box2 worldbox; // Offset if position component exists
	Vec2 worldspawn;
	if (e->position) {
		worldbox   = box2_Offset(e->misc->respawn->trigger_box, e->position->position);
		worldspawn = vec2_Add(e->misc->respawn->respawn_pos, e->position->position);
	} else {
		worldbox   = e->misc->respawn->trigger_box;
		worldspawn = e->misc->respawn->respawn_pos;
	}

	if (box2_Contains(worldbox, p->super->position->position))
		p->hazardRespawn = worldspawn;
}

void misc_thinker_Textbox(App *app, Entity *e, Duration deltaTime) {
	if (!e->misc || !e->misc->textbox) {
		WARN("called on an entity without misc or misc.textbox", 0);
		return;
	}
	misc_Textbox *t = e->misc->textbox;
	if (app->player->player == NULL) { // No player, fade out
		t->progress = fmaxf(0.0f, t->progress - duration_Seconds(deltaTime) * MISC_TEXTBOX_FADEIN_SPEED);
		return;
	}
	Component_Player *p = app->player->player;

	Box2 worldbox; // Offset if position component exists
	ASSERT(e->position && "Textboxes must have a position component");
	worldbox = box2_Offset(t->trigger_box, e->position->position);

	if (box2_Contains(worldbox, p->super->position->position))
		// Fade in
		t->progress = fminf(1.0f, t->progress + duration_Seconds(deltaTime) * MISC_TEXTBOX_FADEIN_SPEED);
	else
		// Fade out
		t->progress = fmaxf(0.0f, t->progress - duration_Seconds(deltaTime) * MISC_TEXTBOX_FADEIN_SPEED);
}


// Utility functions for creating misc entities
void misc_InstantiateTextbox(App *app, Entity *e, const char *text, Box2 trigger_box, float offset) {
	ASSERT(e->misc == NULL && "Instantiate must be called with e.misc not set yet");
	ASSERT(e->render == NULL && "Instantiate must be called with e.render not set yet");
	e->misc          = zero_malloc(sizeof(Component_Misc));
	e->misc->textbox = zero_malloc(sizeof(misc_Textbox));

	e->misc->textbox->text        = copy_malloc(text);
	e->misc->textbox->trigger_box = trigger_box;
	e->misc->textbox->offset      = offset;

	e->render  = render_NewComponentFunc(app, &misc_render_Textbox, NULL);
	e->thinker = &misc_thinker_Textbox;
}
