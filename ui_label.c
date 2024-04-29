
#include "types.h"
#include "ui.h"
#include <string.h>


void _ui_Fill_Draw(System_UI *sys, ui_Part *part, uintptr_t user);  // Defined in ui_render.cpp
void _ui_Label_Draw(System_UI *sys, ui_Part *part, uintptr_t user); // Defined in ui_render.cpp
void _ui_Label_Free(System_UI *sys, ui_Part *part, uintptr_t user) {
	ui_Label *label = (ui_Label *)part->user;
	if (label->label)
		free(label->label);
}

ui_Part ui_Label_New(Box2 box, const char *label_text, int alignment) {
	ui_Label *label = zero_malloc(sizeof(ui_Label));
	if (label_text && strlen(label_text) > 0)
		label->label = copy_malloc(label_text);
	label->align = alignment;

	ui_Part part = {
		.box      = box,
		.progress = .0f,
		.hovered  = false,
		.user     = (uintptr_t)label,
		.draw     = &_ui_Label_Draw,
		.update   = NULL,
		.free     = &_ui_Label_Free};
	return part;
}
void ui_Label_SetLabel(ui_Part *part, const char *label_text) {
	ui_Label *label = (ui_Label *)part->user;
	size_t    len;
	if (label_text && (len = strlen(label_text)) > 0) {
		if (label->label) {
			label->label = realloc(label->label, len + 1);
			memcpy(label->label, label_text, len + 1);
		} else
			label->label = copy_malloc(label_text);
	} else {
		if (label->label) {
			free(label->label);
			label->label = NULL;
		}
	}
}

ui_Part ui_Fill_New(Box2 box, uint32_t color) {
	ui_Part part = {
		.box      = box,
		.progress = .0f,
		.hovered  = false,
		.user     = (uintptr_t)color,
		.draw     = &_ui_Fill_Draw,
		.update   = NULL,
		.free     = NULL};
	return part;
}
