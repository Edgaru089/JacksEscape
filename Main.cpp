#include <cwchar>
#include <graphics.h>
#include <stdio.h>

#include "App.h"
#include "Types.h"


int main() {
	TimePoint startup = time_Now();
	TimePoint lastFrame, lastUpdate, frameCounter;
	lastFrame = lastUpdate = frameCounter = time_Now();
	int frameCount                        = 0;

	initgraph(1280, 720);

	App *app = app_NewApp();
	while (!app->wantQuit) {
		if (time_Since(frameCounter).microseconds >= 1000000) { // 1 sec
			Duration d = time_Reset(&frameCounter);
			fprintf(stderr, "[Main] %d frames in the last %.4lf seconds\n", frameCount, duration_Seconds(d));
			frameCount = 0;
		}
		frameCount++;

		app_Advance(app, time_Reset(&lastUpdate));

		app_Render();

		Duration toSleep = {.microseconds = 1000000 / 30 - time_Reset(&lastFrame).microseconds};
		duration_Sleep(toSleep);
	}

	closegraph();
	printf("%.6lf seconds has elapsed\n", duration_Seconds(time_Since(startup)));
	return 0;
}
