
#include "input.h"
#include "types.h"
#include "ui.h"
#include "app.h"


#ifndef RGB
#define RGB(r, g, b) ((r) | ((g) << 8) | ((b) << 16))
#endif


void _ui_Button_Draw(System_UI *sys, ui_Part *part, uintptr_t user); // Defined in ui_render.cpp
void _ui_Button_Update(System_UI *sys, ui_Part *part, uintptr_t user, Duration deltatime) {
	ui_Button *b = (ui_Button *)user;
	if (part->hovered && sys->super->input->keys[input_Key_LeftMouse] == JustReleased)
		b->callback(sys, part, b->callback_data);
}
void _ui_Button_Free(System_UI *sys, ui_Part *part, uintptr_t user) {
	ui_Button *b = (ui_Button *)user;
	free(b->label);
	free(b);
}


ui_Part ui_Button_New(Box2 box, const char *label, ui_ActionCallback callback, uintptr_t data) {
	ui_Button *b     = zero_malloc(sizeof(ui_Button));
	b->callback      = callback;
	b->callback_data = data;
	b->bg            = UI_BUTTON_BACKGROUND;
	b->bg_hover      = UI_BUTTON_HOVERED;
	b->clicked       = UI_BUTTON_CLICKED;
	b->label         = copy_malloc(label);
	b->left_aligned  = false;

	ui_Part part = {
		.box      = box,
		.progress = 0.0f,
		.user     = (uintptr_t)b,
		.draw     = &_ui_Button_Draw,
		.update   = &_ui_Button_Update,
		.free     = &_ui_Button_Free};
	return part;
}
ui_Part ui_Button_NewLeftAligned(Box2 box, const char *label, ui_ActionCallback callback, uintptr_t data);
