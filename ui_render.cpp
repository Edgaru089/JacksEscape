
#include "input.h"
#include "render_util.h"
#include "types.h"
#include "ui.h"
#include "app.h"

#include <easyx.h>


extern "C" void ui_Render(System_UI *sys) {
	// Background filled rectangle
	if (sys->bg.size.x > EPS && sys->bg.size.y > EPS) {
		setfillcolor(UI_BACKGROUND);
		solidrectangle(
			(int)round(sys->bg.lefttop.x),
			(int)round(sys->bg.lefttop.y),
			(int)round(sys->bg.lefttop.x + sys->bg.size.x),
			(int)round(sys->bg.lefttop.y + sys->bg.size.y));
	}
	if (vector_Size(sys->state) == 0)
		return;

	ui_State layer = *(ui_State *)vector_Back(sys->state);
	// Title string
	setbkcolor(UI_BACKGROUND);
	render_DrawText(
		(int)round(sys->bg.lefttop.x),
		(int)round(sys->bg.lefttop.y) - TEXTHEIGHT,
		ui_StateTitle[layer]);

	// UI elements
	// Skip them if the width of background is less than 90% of the target
	if (sys->bg.size.x > sys->bg_target.size.x * 0.9)
		for (int i = 0; i < vector_Size(sys->parts[layer]); i++) {
			ui_Part *part = (ui_Part *)vector_At(sys->parts[layer], i);
			if (part->draw)
				part->draw(sys, part, part->user);
		}
}

extern "C" void _ui_Button_Draw(System_UI *sys, ui_Part *part, void *user) {
	ui_Button *b = (ui_Button *)user;

	uint32_t color_final = 0;

	if (part->hovered && input_IsPressed(sys->super->input->keys[input_Key_LeftMouse]))
		// Hovered & pressed
		color_final = b->clicked;
	else {
		// Fade the color
		int r0 = b->bg & 0xff;
		int g0 = (b->bg & 0xff00) >> 8;
		int b0 = (b->bg & 0xff0000) >> 16;

		int r1 = b->bg_hover & 0xff;
		int g1 = (b->bg_hover & 0xff00) >> 8;
		int b1 = (b->bg_hover & 0xff0000) >> 16;

		color_final = RGB(
			(int)((r1 - r0) * sqrt(part->progress) + r0),
			(int)((g1 - g0) * sqrt(part->progress) + g0),
			(int)((b1 - b0) * sqrt(part->progress) + b0));
	}

	setbkcolor(color_final);
	setfillcolor(color_final);
	settextcolor(RGB(255, 255, 255));

	solidrectangle(
		(int)round(part->box.lefttop.x),
		(int)round(part->box.lefttop.y),
		(int)round(part->box.lefttop.x + part->box.size.x),
		(int)round(part->box.lefttop.y + part->box.size.y));
	if (b->left_aligned) {
		Box2 padded_box = part->box;
		padded_box.lefttop.x += UI_PADDING;
		padded_box.size.x -= 2 * UI_PADDING;

		render_DrawTextEx(b->label, padded_box, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	} else
		render_DrawTextEx(b->label, part->box, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

extern "C" void _ui_Label_Draw(System_UI *sys, ui_Part *part, uintptr_t user) {
	ui_Label *label = (ui_Label *)user;
	if (!label->label || strlen(label->label) == 0)
		return;

	Box2 padded_box = part->box;
	padded_box.lefttop.x += UI_PADDING;
	padded_box.size.x -= 2 * UI_PADDING;

	setbkcolor(0);
	settextcolor(RGB(255, 255, 255));

	if (label->align < 0)
		render_DrawTextEx(label->label, padded_box, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	else if (label->align > 0)
		render_DrawTextEx(label->label, padded_box, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	else // align == 0
		render_DrawTextEx(label->label, padded_box, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

extern "C" void _ui_Fill_Draw(System_UI *sys, ui_Part *part, uintptr_t user) {
	setfillcolor(part->user);
	solidrectangle(
		(int)round(part->box.lefttop.x),
		(int)round(part->box.lefttop.y),
		(int)round(part->box.lefttop.x + part->box.size.x),
		(int)round(part->box.lefttop.y + part->box.size.y));
}
