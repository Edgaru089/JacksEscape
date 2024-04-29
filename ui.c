
#include "ui.h"
#include "input.h"
#include "app.h"
#include "types.h"
#include "util/vector.h"
#include <stdio.h>


const char *ui_StateTitle[ui_StateCount] = {
	"Running",
	"Pause Menu",
	"Jack's Escape (ver. 3.141592) - Title",
	"Select Level",
	"Options",
	"Level Finished",
	"Leaderboards"};


static inline double dabs(double x) { return x > 0 ? x : -x; }
static inline double dmin(double x, double y) { return x < y ? x : y; }
static inline double dmax(double x, double y) { return x > y ? x : y; }


System_UI *ui_NewSystem(App *super) {
	System_UI *sys = zero_malloc(sizeof(System_UI));
	sys->super     = super;
	sys->state     = vector_Create(sizeof(ui_State));
	for (int i = 0; i < ui_StateCount; i++)
		sys->parts[i] = vector_Create(sizeof(ui_Part));
	return sys;
}
void ui_DeleteSystem(System_UI *sys) {
	for (int i = 0; i < ui_StateCount; i++)
		for (int j = 0; j < vector_Size(sys->parts[i]); j++) {
			ui_Part *part = (ui_Part *)vector_At(sys->parts[i], j);
			if (part->free)
				part->free(sys, part, part->user);
		}
	for (int i = 0; i < ui_StateCount; i++)
		vector_Destroy(sys->parts[i]);
	vector_Destroy(sys->state);
	free(sys);
}


void ui_PushPart(System_UI *sys, ui_State layer, ui_Part part) {
	vector_Push(sys->parts[layer], &part);
}


static inline double fadeto(double from, double to, Duration deltaTime) {
	if (dabs(from - to) < EPS)
		return to;
	else
		return from + (to - from) * UI_BACKGROUND_FADE_MOVE_SPEED * duration_Seconds(deltaTime);
}


static void _ui_UpdateBgTarget(System_UI *sys) {
	if (vector_Size(sys->state) == 0) {
		WARN("state stack is empty");
		return;
	}

	ui_State layer = *(ui_State *)vector_Back(sys->state);
	if (vector_Size(sys->parts[layer]) == 0) {
		// No UI target, set to (0,0), (0,0)
		memset(&sys->bg_target, 0, sizeof(sys->bg_target));
		return;
	}

	Vec2 min = {.x = SCREEN_WIDTH, .y = SCREEN_HEIGHT}, max = {.x = 0, .y = 0};
	for (int i = 0; i < vector_Size(sys->parts[layer]); i++) {
		ui_Part *part = (ui_Part *)vector_At(sys->parts[layer], i);
		min.x         = dmin(min.x, part->box.lefttop.x);
		min.y         = dmin(min.y, part->box.lefttop.y);
		max.x         = dmax(max.x, part->box.lefttop.x + part->box.size.x);
		max.y         = dmax(max.y, part->box.lefttop.y + part->box.size.y);
	}
	sys->bg_target.lefttop = vec2(min.x - UI_PADDING, min.y - UI_PADDING);
	sys->bg_target.size    = vec2(max.x - min.x + 2 * UI_PADDING, max.y - min.y + 2 * UI_PADDING);
}

void ui_Advance(System_UI *sys, Duration deltaTime) {
	Vec2 lt = sys->bg.lefttop, rb = vec2_Add(sys->bg.lefttop, sys->bg.size);
	Vec2 lt_target = sys->bg_target.lefttop, rb_target = vec2_Add(sys->bg_target.lefttop, sys->bg_target.size);
	lt.x = fadeto(lt.x, lt_target.x, deltaTime);
	lt.y = fadeto(lt.y, lt_target.y, deltaTime);
	rb.x = fadeto(rb.x, rb_target.x, deltaTime);
	rb.y = fadeto(rb.y, rb_target.y, deltaTime);

	sys->bg.lefttop = lt;
	sys->bg.size    = vec2_Minus(rb, lt);

	ui_State state = ui_CurrentState(sys);
	for (int i = 0; i < vector_Size(sys->parts[state]); i++) {
		ui_Part *part = (ui_Part *)vector_At(sys->parts[state], i);
		// Hover check
		if (box2_Contains(part->box, input_MousePosition(sys->super->input))) {
			part->hovered  = true;
			part->progress = dmin(part->progress + UI_BUTTON_HOVER_SPEED * duration_Seconds(deltaTime), 1.0);
		} else {
			part->hovered  = false;
			part->progress = dmax(part->progress - UI_BUTTON_HOVER_SPEED * duration_Seconds(deltaTime), 0.0);
		}

		if (part->update)
			part->update(sys, part, part->user, deltaTime);
	}
}


void ui_PushState(System_UI *sys, ui_State push_state) {
	vector_Push(sys->state, &push_state);
	_ui_UpdateBgTarget(sys);
}
void ui_PopState(System_UI *sys) {
	if (vector_Size(sys->state) == 0)
		WARN("state stack empty");
	else {
		vector_Pop(sys->state, NULL);
		_ui_UpdateBgTarget(sys);
	}
}

ui_State ui_CurrentState(System_UI *sys) {
	if (vector_Size(sys->state) == 0)
		WARN("state stack empty");
	return *(ui_State *)vector_Back(sys->state);
}
