#pragma once

#include "entity.h"
#include "gametime.h"
#include "leaderboards.h"
#include "physics.h"
#include "player.h"
#include "input.h"
#include "camera.h"
#include "particle.h"
#include "ui.h"
#include "types.h"
#include "util/vector.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _App {
	void *window; // HWND pointer; window handle set by main()

	System_Physics      *physics;
	System_Player       *player;
	System_Input        *input;
	System_Entity       *entity;
	System_Camera       *camera;
	System_Particle     *particle;
	System_GameTime     *time;
	System_UI           *ui;
	System_Leaderboards *lboard;

	char    *current_level;
	char    *switch_level;
	Duration level_playtime;
	double   timescale;
	uint32_t clear_color;
	bool     wantQuit, debugOn;
	bool     paused;
} App;

App *app_NewApp();
void app_DeleteApp(App *app);

void app_Advance(App *app, Duration deltaTime);
void app_Render(App *app);

void app_DebugText(App *app, vector_Vector *vec_string);

// Trigger the app to reload all entities before next frame update.
void app_QueueLoadLevel(App *app, const char *level_name);
void _app_SwitchLevel(App *app);


#ifdef __cplusplus
}
#endif
