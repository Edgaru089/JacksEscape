
#include "mapper_misc.h"
#include "app.h"
#include "entity.h"
#include "render_util.h"
#include "util/assert.h"
#include <stdio.h>
#include <math.h>
#include <easyx.h>


#ifdef __MINGW32__
#define NCHAR char
#else
#define NCHAR wchar_t
#endif

namespace {
static vector_Vector *tbuf;

void convert_text(const char *str) {
	if (!tbuf)
		tbuf = vector_Create(sizeof(NCHAR));

	const NCHAR zero = 0;

	vector_Clear(tbuf);
	int len = strlen(str);
	int i   = 0;
	while (i < len) {
		NCHAR wc = str[i];
		vector_Push(tbuf, &wc);
		i++;
	}

	vector_Push(tbuf, &zero);
}
} // namespace


extern "C" void misc_render_Textbox(App *app, Entity *e, Vec2 entity_screen_pos, void *user) {
	if (!e->misc || !e->misc->textbox) {
		WARN("called on an entity without misc or misc.textbox", 0);
		return;
	}
	misc_Textbox *t = e->misc->textbox;

	// If we're too dim to see, quit early
	if (t->progress < EPS)
		return;

	// Set the color
	int rgb = (int)round(fminf(t->progress, 1.0f) * 255.0);
	settextcolor(RGB(rgb, rgb, rgb));

	// Compute the bounding rect
	RECT rect;
	ASSERT(e->position && "Textboxes must have a position component");
	rect.left  = (LONG)round(entity_screen_pos.x) - 20;
	rect.right = (LONG)round(entity_screen_pos.x) + 20;
	rect.top   = (LONG)round(entity_screen_pos.y) + t->offset;
	rect.top   = (LONG)round(entity_screen_pos.y) + t->offset + 40;

	// Convert & draw
	// https://docs.easyx.cn/zh-cn/drawtext
	convert_text(t->text);
	drawtext((LPCTSTR)vector_Data(tbuf), &rect, DT_CENTER | DT_NOCLIP);
}
