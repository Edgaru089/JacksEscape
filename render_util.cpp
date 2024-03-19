
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

void render_DrawText(int x, int y, const char *str) {
	if (!tbuf)
		tbuf = vector_Create(1);

	int        cx = x, cy = y;
	const char zero = 0;

	vector_Clear(tbuf);
	int len = strlen(str);
	int i   = 0;
	while (i < len) {
		if (str[i] == '\n') {
			vector_Push(tbuf, &zero);
			outtextxy(cx, cy, (LPCTSTR)vector_Data(tbuf));

			cy += TEXTHEIGHT;
			vector_Clear(tbuf);
		} else
			vector_Push(tbuf, &str[i]);
		i++;
	}

	if (vector_Size(tbuf) > 0) {
		vector_Push(tbuf, &zero);
		outtextxy(cx, cy, (LPCTSTR)vector_Data(tbuf));
		vector_Clear(tbuf);
	}
}


const FillMode render_DefaultMode = {
	.rop2     = R2_COPYPEN,
	.style    = BS_SOLID,
	.hatch    = 0,
	.rotate   = {.microseconds = 0},
	.dissolve = {.microseconds = 0}};


void render_SetModes(FillMode mode, TimePoint since) {
	if (mode.dissolve.microseconds != 0) {
		// Dissolve mode
		double progress = duration_Seconds(time_Since(since)) / duration_Seconds(mode.dissolve);
		setfillstyle(render_DissolvePattern(progress)); // progress is capped into [0,1] in this func
		setrop2(R2_COPYPEN);
		return;
	}

	if (mode.rotate.microseconds != 0) {
		// Rotate mode
		int steps = round(duration_Seconds(time_Since(since)) / duration_Seconds(mode.dissolve));

		static const long hatches[] = {HS_HORIZONTAL, HS_FDIAGONAL, HS_VERTICAL, HS_BDIAGONAL};
		setfillstyle(BS_HATCHED, hatches[steps % 4], NULL);
		setrop2(R2_COPYPEN);
		return;
	}

	// Normal mode
	setfillstyle(mode.style, mode.hatch, NULL);
	setrop2(mode.rop2);
}

void render_FillScreen(FillMode mode, TimePoint since) {
	fillrectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void render_FillRectW(App *app, Box2 rect, FillMode mode, TimePoint since) {
	if (app->camera)
		rect = camera_TransformBox2(app->camera, rect);
	else
		fprintf(stderr, "[WARN][render_FillRectW] called without an active camera system\n");
	fillrectangle(
		(int)round(rect.lefttop.x),
		(int)round(rect.lefttop.y),
		(int)round(rect.lefttop.x + rect.size.x),
		(int)round(rect.lefttop.y + rect.size.y));
}

void render_FillCircleW(App *app, Vec2 center, double radius, FillMode mode, TimePoint since) {
	if (app->camera) {
		center = camera_TransformVec2(app->camera, center);
		radius = camera_TransformSize(app->camera, vec2(radius, 0)).x; // TODO non-aspect scaling
	} else
		fprintf(stderr, "[WARN][render_FillCircleW] called without an active camera system\n");
	fillcircle(
		(int)round(center.x),
		(int)round(center.y),
		(int)round(radius));
}
}
