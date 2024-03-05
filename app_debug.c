
#include "app.h"
#include "util/vector.h"

#include <stdio.h>


#define PUSH_STRING(str) vector_Append(vec_string, str, strlen(str));

void app_DebugText(App *app, vector_Vector *vec_string) {
	vector_Clear(vec_string);
	char buf[256] = {};

	Component_Player *player = app->player->player;
	if (!player) {
		PUSH_STRING("Player not available\n");
	} else {
		snprintf(
			buf, sizeof(buf) - 1,
			"Player:\n    Pos: [%.4lf, %.4lf], Vec:[%.4lf, %.4lf]\n",
			player->super->position->position.x,
			player->super->position->position.y,
			player->super->position->velocity.x,
			player->super->position->velocity.y);
		PUSH_STRING(buf);
	}

	char zero = '\0';
	vector_Push(vec_string, &zero);
}
