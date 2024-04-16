
#include "app.h"
#include "types.h"
#include "util/tree.h"
#include "util/vector.h"

#include <stdio.h>


#define PUSH_STRING(str) vector_Append(vec_string, str, strlen(str));

void app_DebugText(App *app, vector_Vector *vec_string) {
	vector_Clear(vec_string);
	char buf[256];
	memset(buf, 0, sizeof(0));

	Component_Player *player = app->player->player;
	if (!player) {
		PUSH_STRING("Player not available\n");
	} else {
		snprintf(
			buf, sizeof(buf) - 1,
			"Player:\n"
			"    Pos: [%.4lf, %.4lf]\n"
			"    Vec: [%.4lf, %.4lf]\n"
			"    OnGround: %s\n"
			"    JumpCount: %d\n",
			player->super->position->position.x,
			player->super->position->position.y,
			player->super->position->velocity.x,
			player->super->position->velocity.y,
			(player->onGround ? "true" : "false"),
			player->jumpCount);
		PUSH_STRING(buf);
	}

	Vec2 center = box2_Center(app->camera->cam),
		 size   = app->camera->cam.size;
	snprintf(
		buf, sizeof(buf) - 1,
		"Camera:\n"
		"    Center: [%.4lf, %.4lf]\n"
		"    Size:   [%.4lf, %.4lf]\n",
		center.x, center.y,
		size.x, size.y);
	PUSH_STRING(buf);
	snprintf(
		buf, sizeof(buf) - 1,
		"Particle count[0]: %d\n", tree_Count(app->particle->parts[0]));
	PUSH_STRING(buf);

	char zero = '\0';
	vector_Push(vec_string, &zero);
}
