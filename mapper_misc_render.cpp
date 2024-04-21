
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

	// This is the background
	int r = app->clear_color & 0xff;
	int g = (app->clear_color & 0xff00) >> 8;
	int b = (app->clear_color & 0xff0000) >> 16;

	// Fade foreground from App bg to White
	settextcolor(RGB((int)roundf(r + (255 - r) * t->progress), (int)roundf(g + (255 - g) * t->progress), (int)roundf(b + (255 - b) * t->progress)));

	// Fade the background from App background to Black
	setbkcolor(RGB((int)roundf(r * (1.0f - t->progress)), (int)roundf(g * (1.0f - t->progress)), (int)roundf(b * (1.0f - t->progress))));


	// Compute the bounding rect
	RECT rect;
	ASSERT(e->position && "Textboxes must have a position component");
	rect.left  = (LONG)round(entity_screen_pos.x) - 20;
	rect.right = (LONG)round(entity_screen_pos.x) + 20;
	rect.top   = (LONG)round(entity_screen_pos.y) + t->offset * 0.8;

	// Convert & draw
	// https://docs.easyx.cn/zh-cn/drawtext
	convert_text(t->text);
	drawtext((LPCTSTR)vector_Data(tbuf), &rect, DT_CENTER | DT_NOCLIP);

	// Restore background to Black
	setbkcolor(0);
}
