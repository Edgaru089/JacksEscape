#pragma once

#include "Entity.h"
#include "Physics_Component.h"
#include "Player_Component.h"
#include "Input.h"
#include "Types.h"

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


#ifdef __cplusplus
}
#endif
