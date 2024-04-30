
#include "leaderboards.h"
#include "render_util.h"
#include "types.h"
#include "ui.h"
#include "app.h"
#include "util/tree.h"
#include "util/vector.h"
#include <stdio.h>
#include <windows.h>


#ifndef RGB
#define RGB(r, g, b) ((r) | ((g) << 8) | ((b) << 16))
#endif

#define UI_LEADERBOARDS_ITEM_COUNT (8)


// Defined in ui_label.c
void _ui_Label_UpdateShiny(System_UI *sys, ui_Part *part, uintptr_t user, Duration deltatime);


static void _ui_Action_ResumeGame(System_UI *sys, ui_Part *part, uintptr_t data) {
	ui_Action_PopState(sys, part, data);
	sys->super->paused = false;
}
static void _ui_Action_ReturnToTitle(System_UI *sys, ui_Part *part, uintptr_t data) {
	while (vector_Size(sys->state) > 0 && ui_CurrentState(sys) != ui_TitleMenu) {
		INFO("popping \"%s\"", ui_StateTitle[ui_CurrentState(sys)]);
		ui_Action_PopState(sys, part, 0);
	}
	sys->super->paused = false;
	if (vector_Size(sys->state) == 0)
		WARN("new state stack is now empty, ohno");

	sys->super->clear_color = RGB(40, 40, 40);
	app_QueueLoadLevel(sys->super, "title.txt");
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

static void _ui_Action_ReplayLevel(System_UI *sys, ui_Part *part, uintptr_t data) {
	_ui_Action_SelectLevel(sys, part, (uintptr_t)sys->super->current_level);
}

static void _ui_Action_EndIntermission(System_UI *sys, ui_Part *part, uintptr_t data) {
	if (!data)
		// next mission string is NULL, return to title
		_ui_Action_ReturnToTitle(sys, part, 0);
	else {
		// load next level
		// should be able to just use this
		_ui_Action_SelectLevel(sys, part, data);
		free((void *)data);
	}
}

static void _ui_Action_EnterLeaderboards(System_UI *sys, ui_Part *part, uintptr_t data) {
	vector_Vector *parts = sys->parts[ui_Leaderboards];
#define PART(i) ((ui_Part *)vector_At(parts, i))

	int collected = 0;
	int indice    = lboard_FindLevel(sys->super->lboard, sys->super->current_level);
	if (indice != -1) {
		tree_Tree *tree = *(tree_Tree **)vector_At(sys->super->lboard->records, indice);
		for (tree_Node *node = tree_FirstNode(tree); node != NULL; node = tree_Node_Next(node)) {
			uintptr_t      time_millisec = node->key;
			vector_Vector *names         = *(vector_Vector **)node->data;
			if (names)
				for (int i = 0; i < vector_Size(names); i++) {
					char *name = *(char **)vector_At(names, i);
					if (collected < UI_LEADERBOARDS_ITEM_COUNT) {
						char buf[64];
						snprintf(buf, sizeof(buf), "%2d %s", collected + 1, name);
						((ui_Label *)PART(collected * 2)->user)->label = copy_realloc(((ui_Label *)PART(collected * 2)->user)->label, buf);
						snprintf(
							buf, sizeof(buf),
							"%02d:%02d.%02d",
							time_millisec / 1000 / 60,
							time_millisec / 1000 % 60,
							time_millisec / 10 % 100);
						((ui_Label *)PART(collected * 2 + 1)->user)->label = copy_realloc(((ui_Label *)PART(collected * 2 + 1)->user)->label, buf);
						collected++;
					}
				}
		}
	}

	while (collected < UI_LEADERBOARDS_ITEM_COUNT) {
		char buf[64];
		snprintf(buf, sizeof(buf), "%2d", collected + 1);
		((ui_Label *)PART(collected * 2)->user)->label     = copy_realloc(((ui_Label *)PART(collected * 2)->user)->label, buf);
		((ui_Label *)PART(collected * 2 + 1)->user)->label = copy_realloc(((ui_Label *)PART(collected * 2 + 1)->user)->label, "-- N/A --");
		collected++;
	}

	ui_Action_PushState(sys, part, (uintptr_t)ui_Leaderboards);
}


void ui_EnterIntermission(System_UI *sys, const char *next_level) {
	// Let's malloc & copy this next_level string
	// And hide it in the last part of this state
	// It must be a button, so put the string in the callback data
	ui_Part *last_part = vector_Back(sys->parts[ui_InterMission]);
	if (((ui_Button *)last_part->user)->callback_data)
		free((void *)((ui_Button *)last_part->user)->callback_data);
	if (!next_level || strlen(next_level) == 0) {
		((ui_Button *)last_part->user)->callback_data = 0;
		((ui_Button *)last_part->user)->label         = copy_realloc(((ui_Button *)last_part->user)->label, "Return to Title");
	} else {
		((ui_Button *)last_part->user)->callback_data = (uintptr_t)copy_malloc(next_level);
		((ui_Button *)last_part->user)->label         = copy_realloc(((ui_Button *)last_part->user)->label, "Proceed to Next Level");
	}
	ui_PushState(sys, ui_InterMission);
	sys->super->paused = true;

	// We need to update the UI elements in this State
	//  0            1            2             3             4           5              6                 7              8     9
	// "Level Time" <level_time> "Record Time" <record_time> <seperator> <replay_level> <return_to_title> <leaderboards> <sep> <next_level>
	vector_Vector *parts = sys->parts[ui_InterMission];
	// #define PART(i) ((ui_Part *)vector_At(parts, i))
	char buf[64];

	// <level_time>
	snprintf(
		buf, sizeof(buf),
		"%02d:%02d.%02d",
		sys->super->level_playtime.microseconds / 1000 / 1000 / 60,
		sys->super->level_playtime.microseconds / 1000 / 1000 % 60,
		sys->super->level_playtime.microseconds / 10000 % 100);
	ui_Label_SetLabel(PART(1), buf);

	// Get the record time
	uintptr_t record = 0;
	int       indice = lboard_FindLevel(sys->super->lboard, sys->super->current_level);
	if (indice != -1) {
		tree_Tree *tree  = *(tree_Tree **)vector_At(sys->super->lboard->records, indice);
		tree_Node *first = tree_FirstNode(tree);
		if (first)
			record = first->key;
	}

	// <record_time>
	if (record)
		snprintf(
			buf, sizeof(buf),
			"%02d:%02d.%02d",
			record / 1000 / 60,
			record / 1000 % 60,
			record / 10 % 100);
	else
		strcpy(buf, "--:--.--");
	ui_Label_SetLabel(PART(3), buf);

	// Shiny if lower than record
	if (record == 0 || sys->super->level_playtime.microseconds / 1000 < record)
		PART(1)->update = &_ui_Label_UpdateShiny;
	else {
		PART(1)->update                    = NULL;
		((ui_Label *)PART(1)->user)->color = RGB(255, 255, 255);
	}

	// Get user name
	DWORD bufsize = sizeof(buf) - 1;
	GetUserNameA(buf, &bufsize);
	lboard_Insert(sys->super->lboard, sys->super->current_level, sys->super->level_playtime.microseconds / 1000, buf);
	lboard_SaveToFile(sys->super->lboard, "leaderboards.txt");
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
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 250), vec2(400, 40)),
		"Resume Game",
		&_ui_Action_ResumeGame, 0))
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 250 + (40 + UI_PADDING)), vec2(400, 40)),
		"Reselect Level",
		&ui_Action_PushState, (uintptr_t)ui_SceneSelect))
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 250 + 2 * (40 + UI_PADDING)), vec2(400, 40)),
		"Leaderboard for This Level",
		&_ui_Action_EnterLeaderboards, 0))
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 250 + 3 * (40 + UI_PADDING)), vec2(400, 40)),
		"Options",
		&ui_Action_PushState, (uintptr_t)ui_Options))
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 250 + 4 * (40 + UI_PADDING) + UI_PADDING), vec2(400, 40)),
		"Return to Title",
		&_ui_Action_ReturnToTitle, 0))


	// Options
	p = sys->parts[ui_Options];
	vector_Clear(p);
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 300), vec2(600, 50)),
		"Return",
		&ui_Action_PopState, 0))


	// InterMission
	p = sys->parts[ui_InterMission];
	vector_Clear(p);
	ui_BoxBuilder *builder = ui_BoxBuilder_New();
	builder->line_height   = 36;
	builder->pivot         = vec2(0.5, 0.625);
	builder->position      = vec2(SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0);
	ui_BoxBuilder_SetCount(builder, 1, 1);
	ui_BoxBuilder_SetCount(builder, 2, 1);
	ui_BoxBuilder_SetCount(builder, 4, 2);
	ui_BoxBuilder_SetCount(builder, 5, 1);
	ui_BoxBuilder_SetCount(builder, 7, 1);
	ui_BoxBuilder_Assemble(builder);

	PUSH_UI(ui_Label_New(
		ui_BoxBuilder_At(builder, 1, 1),
		"Time in This Level", -1))
	PUSH_UI(ui_Label_New(
		ui_BoxBuilder_At(builder, 1, 1),
		"00:00.00", 1))
	PUSH_UI(ui_Label_New(
		ui_BoxBuilder_At(builder, 2, 1),
		"Record Time", -1))
	PUSH_UI(ui_Label_New(
		ui_BoxBuilder_At(builder, 2, 1),
		"00:00.00", 1))
	PUSH_UI(ui_Fill_New(
		box2v(
			vec2_Add(
				ui_BoxBuilder_At(builder, 4, 1).lefttop,
				vec2(0, -UI_PADDING)),
			vec2(builder->width, 1.0)),
		RGB(128, 128, 128)));
	PUSH_UI(ui_Button_New(
		ui_BoxBuilder_At(builder, 4, 1),
		"Replay Level",
		&_ui_Action_ReplayLevel, 0))
	PUSH_UI(ui_Button_New(
		ui_BoxBuilder_At(builder, 4, 2),
		"Back to Title",
		&_ui_Action_ReturnToTitle, 0))
	PUSH_UI(ui_Button_New(
		ui_BoxBuilder_At(builder, 5, 1),
		"View Leaderboards for Level",
		&_ui_Action_EnterLeaderboards, 0))
	PUSH_UI(ui_Fill_New(
		box2v(
			vec2_Add(
				ui_BoxBuilder_At(builder, 7, 1).lefttop,
				vec2(0, -UI_PADDING)),
			vec2(builder->width, 1.0)),
		RGB(128, 128, 128)));
	PUSH_UI(ui_Button_New(
		ui_BoxBuilder_At(builder, 7, 1),
		"Proceed to Next Level",
		&_ui_Action_EndIntermission, 0))
	ui_BoxBuilder_Delete(builder);


	// Leaderboards
	p = sys->parts[ui_Leaderboards];
	vector_Clear(p);

	builder              = ui_BoxBuilder_New();
	builder->line_height = 28;
	builder->pivot       = vec2(0.5, 0.5625);
	builder->position    = vec2(SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0);
	for (int i = 1; i <= UI_LEADERBOARDS_ITEM_COUNT; i++)
		ui_BoxBuilder_SetCount(builder, i, 1);
	ui_BoxBuilder_SetCount(builder, UI_LEADERBOARDS_ITEM_COUNT + 1, 1);
	ui_BoxBuilder_Assemble(builder);

	for (int i = 1; i <= UI_LEADERBOARDS_ITEM_COUNT; i++) {
		char buf[64];
		snprintf(buf, sizeof(buf), "%2d", i);
		PUSH_UI(ui_Label_New(
			ui_BoxBuilder_At(builder, i, 1),
			buf, -1))
		PUSH_UI(ui_Label_New(
			ui_BoxBuilder_At(builder, i, 1),
			"-- N/A --", 1))
	}
	PUSH_UI(ui_Button_New(
		ui_BoxBuilder_At(builder, UI_LEADERBOARDS_ITEM_COUNT + 1, 1),
		"Return",
		&ui_Action_PopState, 0))
}
