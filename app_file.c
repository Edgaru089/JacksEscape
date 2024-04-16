
#include <stdio.h>
#include "app.h"
#include "entity.h"
#include "mapper_misc.h"
#include "particle.h"
#include "physics.h"
#include "render_component.h"
#include "types.h"
#include "util/vector.h"
#include "util/assert.h"


void app_QueueLoadLevel(App *app, const char *level_name) {
	if (app->switch_level != NULL) {
		WARN("previous switch_level \"%s\" not processed; purged", app->switch_level);
		free(app->switch_level);
	}
	app->switch_level = copy_malloc(level_name);
}


#define CMD1(str)    if (strcmp(cmd, str) == 0)
#define CMD(str)     else if (strcmp(cmd, str) == 0)
#define TOKEN        (strtok(NULL, " "))
#define TOKEN_INT    (atoi(TOKEN))
#define TOKEN_DOUBLE (strtod(TOKEN, NULL))


static vector_Vector *charbuf;

static void _app_UnescapeTextbox(char *t) {
	ASSERT(t && "only can be called with t!=NULL");
	if (!charbuf)
		charbuf = vector_Create(sizeof(char));
	else
		vector_Clear(charbuf);
	size_t len = strlen(t);

	const char bs = '\\', space = ' ', newline = '\n', tab = '\t', zero = '\0';

	for (int i = 0; i < len; i++) {
		if (t[i] == '\\') {
			if (i == len - 1)
				vector_Push(charbuf, &bs); // Weird case
			else {
				if (t[i + 1] == 's')
					vector_Push(charbuf, &space);
				else if (t[i + 1] == 'n')
					vector_Push(charbuf, &newline);
				else if (t[i + 1] == 't')
					vector_Push(charbuf, &tab);
				i++;
			}
		} else
			vector_Push(charbuf, &t[i]);
	}
	vector_Push(charbuf, &zero);
}

static inline Vec2 readvec2() {
	double a, b;
	a      = TOKEN_DOUBLE;
	b      = TOKEN_DOUBLE;
	Vec2 v = {.x = a, .y = b};
	return v;
}
static inline Box2 readbox2() {
	Vec2 a, b;
	a        = readvec2();
	b        = readvec2();
	Box2 box = {.lefttop = a, .size = b};
	return box;
}
static inline uint32_t readcolor() {
	int r, g, b;
	r = TOKEN_INT;
	g = TOKEN_INT;
	b = TOKEN_INT;
	return (uint32_t)((r) | (g << 8) | (b << 16));
}


// Subsequent tokens can be read by strtok(NULL, " ")
static void _app_LevelCommand(App *app, char *cmd) {
	CMD1("HITBOX") {
		Box2 box = readbox2();

		Entity *e = entity_Create(app->entity, cmd);
		ADD_COMPONENT(e, hitbox);
		e->hitbox->box   = box;
		e->hitbox->fixed = true;
		entity_Commit(app->entity, e);
	}

	CMD("PLAYER") {
		Vec2 vec = readvec2();

		Entity *e = entity_Create(app->entity, cmd);
		ADD_COMPONENT(e, player);
		e->player->hazardRespawn = vec;
		ADD_COMPONENT(e, position);
		e->position->position = vec;
		e->position->velocity = vec2(0, 0);
		ADD_COMPONENT(e, hitbox);
		e->hitbox->box.lefttop = vec2(-20, -50);
		e->hitbox->box.size    = vec2(40, 50);
		entity_Commit(app->entity, e);
	}

	CMD("HAZARD_RESPAWN") {
		Box2 box = readbox2();
		Vec2 vec = readvec2();

		Entity *e = entity_Create(app->entity, cmd);
		misc_InstantiateHazardRespawn(app, e, box, vec);
		entity_Commit(app->entity, e);
	}

	CMD("HAZARD") {
		Box2 box = readbox2();

		Entity *e = entity_Create(app->entity, cmd);
		misc_InstantiateHazard(app, e, box);
		entity_Commit(app->entity, e);
	}

	CMD("TEXTBOX") {
		double a, b, c, d;
		a = TOKEN_DOUBLE;
		b = TOKEN_DOUBLE;
		c = TOKEN_DOUBLE;
		d = TOKEN_DOUBLE;

		Entity *e      = entity_Create(app->entity, cmd);
		char   *bundle = TOKEN;
		if (bundle != NULL)
			e->render = render_NewComponent(e, bundle);

		// We need to compute a position element
		Vec2 position = {
			.x = a + c / 2.0,
			.y = b + d};
		ADD_COMPONENT(e, position);
		e->position->position = position;
		e->position->velocity = vec2(0.0, 0.0);

		char *text_raw = TOKEN;
		if (text_raw) {
			_app_UnescapeTextbox(text_raw);
			misc_InstantiateTextbox(app, e, vector_Data(charbuf), box2(-c / 2.0, -d, c, d), (-d - 40));
		}
		entity_Commit(app->entity, e);
	}

	CMD("LEVEL_TRANSITION") {
		Box2  box        = readbox2();
		char *next_level = TOKEN;
		if (next_level) {
			Entity *e = entity_Create(app->entity, cmd);
			misc_InstantiateChangeLevel(app, e, box, next_level);
			entity_Commit(app->entity, e);
		}
	}

	CMD("CAMERA_FOCUS") {
		Box2 box = readbox2();

		Entity *e = entity_Create(app->entity, cmd);
		misc_InstantiateCameraFocus(app, e, box);
		entity_Commit(app->entity, e);
	}

	CMD("CUTOFF") {
		app->player->cutoff = TOKEN_DOUBLE;
	}

	CMD("BACKGROUND") {
		app->clear_color = readcolor();
	}

	CMD("FILL") {
		uint32_t color = readcolor();
		Box2     box   = readbox2();

		Entity *e = entity_Create(app->entity, cmd);
		ADD_COMPONENT(e, render);
		e->render->fillbox   = box;
		e->render->fillcolor = color;
	}

	else {
		WARN("unknown command \"%s\"", cmd);
	}
}


// Defined in render_bundle_file.c
extern char linebuf[512];

void _app_SwitchLevel(App *app) {
	if (app->switch_level == NULL) {
		WARN("called when switch_level is NULL", 0);
		return;
	}
	INFO("Switching level to %s", app->switch_level);

	FILE *f = fopen(app->switch_level, "r");
	if (!f) {
		WARN("failed to open file\"%s\"", app->switch_level);
		return;
	}

	// Clear the current level
	entity_Clear(app->entity);
	particle_Clear(app->particle);

	// Read every line
	while (!feof(f) && fgets(linebuf, sizeof(linebuf), f)) {
		while (linebuf[strlen(linebuf) - 1] == '\n')
			linebuf[strlen(linebuf) - 1] = '\0';

		char *cmd = strtok(linebuf, " ");
		if (cmd == NULL)
			continue;
		_app_LevelCommand(app, cmd);
	}


	free(app->switch_level);
	app->switch_level = NULL;
	fclose(f);
}
