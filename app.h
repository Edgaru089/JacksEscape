#pragma once

#include "entity.h"
#include "physics.h"
#include "player.h"
#include "input.h"
#include "types.h"
#include "util/vector.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _App {
	System_Physics *physics;
	System_Player  *player;
	System_Input   *input;
	System_Entity  *entity;

	bool wantQuit;
} App;

App *app_NewApp();
void app_DeleteApp(App *app);

void app_Advance(App *app, Duration deltaTime);
void app_Render(App *app);

void app_DebugText(App *app, vector_Vector *vec_string);


#ifdef __cplusplus
}
#endif
