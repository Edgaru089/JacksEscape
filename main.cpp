#include <cwchar>
#include <graphics.h>
#include <stdio.h>

#include "app.h"
#include "easyx.h"
#include "types.h"
#include "render_util.h"
#include "util/vector.h"


int main() {
	TimePoint startup = time_Now();
	TimePoint lastFrame, lastUpdate, frameCounter;
	lastFrame = lastUpdate = frameCounter = time_Now();
	int frameCount                        = 0;

#ifdef __MINGW32__
	SetProcessDPIAware();
#endif
	HWND win = initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetWindowTextA(win, "JacksEscape");

#ifdef __MINGW32__
	settextstyle(TEXTHEIGHT, 0, "Courier New");
#else // MSVC
	settextstyle(TEXTHEIGHT, 0, L"Courier New");
#endif
	vector_Vector *debugText = vector_Create(1);

	App *app = app_NewApp();
	while (!app->wantQuit) {
		if (time_Since(frameCounter).microseconds >= 1000000) { // 1 sec
			/* Duration d = */ time_Reset(&frameCounter);
			char buf[128];
			snprintf(buf, sizeof(buf) - 1, "JacksEscape (%d FPS)", frameCount);
			SetWindowTextA(win, buf);
			frameCount = 0;
		}
		frameCount++;

		app_Advance(app, time_Reset(&lastUpdate));

		BeginBatchDraw();
		cleardevice();
		app_Render(app);

		app_DebugText(app, debugText);
		render_DrawText(10, 10, (const char *)vector_Data(debugText));

		EndBatchDraw();


		Duration toSleep = {.microseconds = 1000000 / 200 - time_Reset(&lastFrame).microseconds};
		duration_Sleep(toSleep);
	}

	closegraph();
	app_DeleteApp(app);
	printf("%.6lf seconds has elapsed\n", duration_Seconds(time_Since(startup)));
	return 0;
}
