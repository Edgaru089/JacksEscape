#include <cwchar>
#include <graphics.h>
#include <stdio.h>

#include "app.h"
#include "types.h"


int main() {
	TimePoint startup = time_Now();
	TimePoint lastFrame, lastUpdate, frameCounter;
	lastFrame = lastUpdate = frameCounter = time_Now();
	int frameCount                        = 0;

	HWND win = initgraph(1280, 720);
	SetWindowTextA(win, "JacksEscape");

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
		EndBatchDraw();

		Duration toSleep = {.microseconds = 1000000 / 60 - time_Reset(&lastFrame).microseconds};
		duration_Sleep(toSleep);
	}

	closegraph();
	printf("%.6lf seconds has elapsed\n", duration_Seconds(time_Since(startup)));
	return 0;
}
