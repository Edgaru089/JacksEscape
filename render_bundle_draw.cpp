
#include "camera.h"
#include "render_bundle.h"
#include "app.h"
#include "render_util.h"
#include "util/vector.h"
#include <stdio.h>
#include <math.h>
#include <easyx.h>
#include <wingdi.h>


namespace {
vector_Vector *buff;
}


extern "C" void render_DrawPrimitiveW(App *app, render_Primitive *p, Vec2 offset) {
	if (buff == NULL)
		buff = vector_Create(sizeof(POINT));
	vector_Clear(buff);

	bool needDraw = false;

	// Construct the points in screen space
	for (int i = 0; i < vector_Size(p->points); i++) {
		Vec2 realpos = vec2_Add(*((Vec2 *)vector_At(p->points, i)), offset);
		Vec2 screenpos;
		if (!app->camera) {
			// Really weird
			WARN("called without a Camera system");
			screenpos = realpos;
			needDraw  = true;
		} else {
			screenpos = camera_TransformVec2(app->camera, realpos);
			needDraw  = needDraw || box2_Contains(app->camera->cam, realpos);
		}


		// Round the screen position to ints
		POINT rounded;
		rounded.x = (LONG)round(screenpos.x);
		rounded.y = (LONG)round(screenpos.y);
		vector_Push(buff, &rounded);
	}

	// See if any of the points are in the camera box
	if (needDraw) {
		// Set the colors
		render_SetModes(p->mode, time_Now());

		// Draw the converted primitive
		switch (p->type) {
			case render_Lines:
				if (vector_Size(buff) % 2 != 0)
					WARN("render_Lines drawed odd numbers of points", 0);
				for (int i = 0; i < vector_Size(buff) - 1; i += 2) {
					POINT p0 = *(POINT *)vector_At(buff, i);
					POINT p1 = *(POINT *)vector_At(buff, i + 1);
					line(p0.x, p0.y, p1.x, p1.y);
				}
				break;
			case render_LineStrip:
				for (int i = 0; i < vector_Size(buff) - 1; i++) {
					POINT p0 = *(POINT *)vector_At(buff, i);
					POINT p1 = *(POINT *)vector_At(buff, i + 1);
					line(p0.x, p0.y, p1.x, p1.y);
				}
				break;
			case render_Polygon:
				fillpolygon((POINT *)vector_Data(buff), vector_Size(buff));
				break;
		}
	}
}
