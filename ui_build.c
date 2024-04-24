
#include "ui.h"
#include "app.h"
#include "util/vector.h"


static void _ui_Action_ResumeGame(System_UI *sys, ui_Part *part, uintptr_t data) {
	ui_Action_PopState(sys, part, data);
	sys->super->paused = false;
}


#define PUSH_UI(part) \
	_p0 = (part);     \
	vector_Push(p, &_p0);

void ui_RebuildUI(System_UI *sys) {
	vector_Vector *p;
	ui_Part        _p0;


	// Paused
	p = sys->parts[ui_Paused];
	vector_Clear(p);
	PUSH_UI(ui_Button_New(
		box2_FromCenter(vec2(SCREEN_WIDTH / 2.0, 300), vec2(400, 50)),
		"Resume Game",
		&_ui_Action_ResumeGame, 0))
}
