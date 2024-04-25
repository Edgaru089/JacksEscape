
#include "ui.h"
#include "app.h"
#include "util/vector.h"
#include <stdio.h>


#ifndef RGB
#define RGB(r, g, b) ((r) | ((g) << 8) | ((b) << 16))
#endif


static void _ui_Action_ResumeGame(System_UI *sys, ui_Part *part, uintptr_t data) {
	ui_Action_PopState(sys, part, data);
	sys->super->paused = false;
}
static void _ui_Action_ReturnToTitle(System_UI *sys, ui_Part *part, uintptr_t data) {
	while (vector_Size(sys->state) > 0 && ui_CurrentState(sys) != ui_TitleMenu) {
		INFO("popping \"%s\"", ui_StateTitle[ui_CurrentState(sys)]);
		ui_Action_PopState(sys, part, data);
	}
	sys->super->paused = false;
	if (vector_Size(sys->state) == 0)
		WARN("new state stack is now empty, ohno");

	sys->super->clear_color = RGB(40, 40, 40);
	// Clear the app
	// see app_file.c:233
	entity_Clear(sys->super->entity);
	particle_Clear(sys->super->particle);
	sys->super->camera->target = NULL;
}
static void _ui_Action_SelectLevel(System_UI *sys, ui_Part *part, uintptr_t data) {
	while (vector_Size(sys->state) > 0 && ui_CurrentState(sys) != ui_TitleMenu) {
		INFO("popping \"%s\"", ui_StateTitle[ui_CurrentState(sys)]);
		ui_Action_PopState(sys, part, data);
	}
	sys->super->paused = false;
	if (vector_Size(sys->state) == 0)
		WARN("new state stack is now empty, ohno");

	const char *level = (const char *)data;
	app_QueueLoadLevel(sys->super, level);
	ui_Action_PushState(sys, part, (uintptr_t)ui_Running);
}
static void _ui_Action_StartFromBeginning(System_UI *sys, ui_Part *part, uintptr_t data) {
	_ui_Action_SelectLevel(sys, part, (uintptr_t) "intro.txt");
}
static void _ui_Action_Quit(System_UI *sys, ui_Part *part, uintptr_t data) {
	sys->super->wantQuit = true;
}


#define PUSH_UI(part) \
	_p0 = (part);     \
	vector_Push(p, &_p0);

void ui_RebuildUI(System_UI *sys) {
	vector_Vector *p;
	ui_Part        _p0;

	// Title Menu
	p = sys->parts[ui_TitleMenu];
	vector_Clear(p);
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 250), vec2(500, 50)),
		"Start from Beginning",
		&_ui_Action_StartFromBeginning, 0))
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 250 + (50 + UI_PADDING)), vec2(500, 50)),
		"Select Level",
		&ui_Action_PushState, ui_SceneSelect))
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0 - (500 - UI_PADDING) / 4.0 - UI_PADDING / 2.0, 250 + 2 * (50 + UI_PADDING)), vec2((500 - UI_PADDING) / 2.0, 40)),
		"Options",
		&ui_Action_PushState, (uintptr_t)ui_Options))
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0 + (500 - UI_PADDING) / 4.0 + UI_PADDING / 2.0, 250 + 2 * (50 + UI_PADDING)), vec2((500 - UI_PADDING) / 2.0, 40)),
		"Quit",
		&_ui_Action_Quit, 0))


	// Select Level
	p = sys->parts[ui_SceneSelect];
	vector_Clear(p);
	PUSH_UI(ui_Button_NewLeftAligned(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 280), vec2(400, 40)),
		"Introduction",
		&_ui_Action_SelectLevel, (uintptr_t) "intro.txt"));
	PUSH_UI(ui_Button_NewLeftAligned(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 280 + (40 + UI_PADDING / 2.0)), vec2(400, 40)),
		"Scene 1",
		&_ui_Action_SelectLevel, (uintptr_t) "level1.txt"));
	PUSH_UI(ui_Button_NewLeftAligned(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 280 + 2 * (40 + UI_PADDING / 2.0)), vec2(400, 40)),
		"Scene 2",
		&_ui_Action_SelectLevel, (uintptr_t) "level2.txt"));
	PUSH_UI(ui_Button_NewLeftAligned(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 280 + 3 * (40 + UI_PADDING / 2.0)), vec2(400, 40)),
		"Scene 3",
		&_ui_Action_SelectLevel, (uintptr_t) "level3.txt"));
	PUSH_UI(ui_Button_NewLeftAligned(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0 + 100, 280 + 3 * (40 + UI_PADDING / 2.0) + (40 + UI_PADDING)), vec2(200, 40)),
		"Return",
		&ui_Action_PopState, 0));


	// Paused
	p = sys->parts[ui_Paused];
	vector_Clear(p);
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 300), vec2(400, 40)),
		"Resume Game",
		&_ui_Action_ResumeGame, 0))
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 300 + (40 + UI_PADDING)), vec2(400, 40)),
		"Reselect Level",
		&ui_Action_PushState, (uintptr_t)ui_SceneSelect))
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 300 + 2 * (40 + UI_PADDING)), vec2(400, 40)),
		"Options",
		&ui_Action_PushState, (uintptr_t)ui_Options))
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 300 + 3 * (40 + UI_PADDING) + UI_PADDING), vec2(400, 40)),
		"Return to Title",
		&_ui_Action_ReturnToTitle, 0))


	// Options
	p = sys->parts[ui_Options];
	vector_Clear(p);
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 300), vec2(600, 50)),
		"Return",
		&ui_Action_PopState, 0))
}
