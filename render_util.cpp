
#include "render_util.h"
#include "camera.h"
#include "render_pattern.h"
#include "easyx.h"
#include "types.h"
#include "util/vector.h"
#include "app.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <wingdi.h>


extern "C" {


static vector_Vector *tbuf;

#ifdef __MINGW32__
#define NCHAR char
#else
#define NCHAR wchar_t
#endif

void render_DrawText(int x, int y, const char *str) {
	if (!tbuf)
		tbuf = vector_Create(sizeof(NCHAR));

	int         cx = x, cy = y;
	const NCHAR zero = 0;

	vector_Clear(tbuf);
	int len = strlen(str);
	int i   = 0;
	while (i < len) {
		if (str[i] == '\n') {
			vector_Push(tbuf, &zero);
			outtextxy(cx, cy, (LPCTSTR)vector_Data(tbuf));

			cy += TEXTHEIGHT;
			vector_Clear(tbuf);
		} else {
			NCHAR wc = str[i];
			vector_Push(tbuf, &wc);
		}
		i++;
	}

	if (vector_Size(tbuf) > 0) {
		vector_Push(tbuf, &zero);
		outtextxy(cx, cy, (LPCTSTR)vector_Data(tbuf));
		vector_Clear(tbuf);
	}
}


const FillMode render_ModeDefault = {
	.rop2     = R2_COPYPEN,
	.style    = BS_SOLID,
	.hatch    = 0,
	.rotate   = {.microseconds = 0},
	.dissolve = {.microseconds = 0},
	.fadein   = false};
const FillMode render_ModeInverse = {
	.rop2     = R2_NOT,
	.style    = BS_SOLID,
	.hatch    = 0,
	.rotate   = {.microseconds = 0},
	.dissolve = {.microseconds = 0},
	.fadein   = false};
extern const FillMode render_ModeRotate = {
	.rop2     = R2_COPYPEN,
	.style    = BS_SOLID,
	.hatch    = 0,
	.rotate   = {.microseconds = 100000},
	.dissolve = {.microseconds = 0},
	.fadein   = false};


void render_SetModes(FillMode mode, TimePoint since) {
	if (mode.dissolve.microseconds != 0) {
		// Dissolve mode
		double progress = duration_Seconds(time_Since(since)) / duration_Seconds(mode.dissolve);
		if (mode.fadein)
			setfillstyle(render_DissolvePatternIn(progress)); // progress is capped into [0,1] in this func
		else
			setfillstyle(render_DissolvePatternOut(progress));
		setrop2(R2_COPYPEN);
		return;
	}

	if (mode.rotate.microseconds != 0) {
		// Rotate mode
		int steps = (int)round(duration_Seconds(time_Since(since)) / duration_Seconds(mode.rotate));

		static const long hatches[] = {HS_HORIZONTAL, HS_FDIAGONAL, HS_VERTICAL, HS_BDIAGONAL};
		setfillstyle(BS_HATCHED, hatches[steps % 4], NULL);
		setrop2(R2_COPYPEN);
		return;
	}

	// Normal mode
	setfillstyle(mode.style, mode.hatch, NULL);
	setrop2(mode.rop2);
}

void render_FillScreen() {
	solidrectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void render_FillRectW(App *app, Box2 rect) {
	if (app->camera)
		rect = camera_TransformBox2(app->camera, rect);
	else
		fprintf(stderr, "[WARN][render_FillRectW] called without an active camera system\n");
	solidrectangle(
		(int)round(rect.lefttop.x),
		(int)round(rect.lefttop.y),
		(int)round(rect.lefttop.x + rect.size.x),
		(int)round(rect.lefttop.y + rect.size.y));
}

void render_FillCircleW(App *app, Vec2 center, double radius) {
	if (app->camera) {
		center = camera_TransformVec2(app->camera, center);
		radius = camera_TransformSize(app->camera, vec2(radius, 0)).x; // TODO non-aspect scaling
	} else
		fprintf(stderr, "[WARN][render_FillCircleW] called without an active camera system\n");
	solidcircle(
		(int)round(center.x),
		(int)round(center.y),
		(int)round(radius));
}
}
