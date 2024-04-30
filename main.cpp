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
#else
	HINSTANCE user32Dll = LoadLibraryA("user32.dll");
	if (user32Dll) {
		typedef BOOL(WINAPI * SetProcessDPIAwareFuncType)();
		SetProcessDPIAwareFuncType setProcessDPIAwareFunc = (SetProcessDPIAwareFuncType)GetProcAddress(user32Dll, "SetProcessDPIAware");
		if (setProcessDPIAwareFunc) {
			if (!setProcessDPIAwareFunc())
				WARN("failed to set process DPI awareness");
		}
		FreeLibrary(user32Dll);
	}
#endif
	HWND win = initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetWindowTextA(win, "JacksEscape");

#ifdef __MINGW32__
	settextstyle(TEXTHEIGHT, 0, "Courier New");
#else // MSVC
	settextstyle(TEXTHEIGHT, 0, L"Courier New");
#endif
	vector_Vector *debugText    = vector_Create(1);
	bool           debugPressed = false;

	App *app    = app_NewApp();
	app->window = win;
	while (!app->wantQuit) {
		// Check if the window is still open
		// On Windows/MinGW, the loop stucks open when the window is closed
		if (!IsWindow(win))
			break;

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
		app_Render(app);

		if (app->debugOn) {
			setbkcolor(0);
			app_DebugText(app, debugText);
			render_DrawText(10, 10, (const char *)vector_Data(debugText));
		}

		EndBatchDraw();

		// Check debug button status
		if ((GetAsyncKeyState(VK_F3) & 0x8000) != 0) {
			if (!debugPressed)
				app->debugOn = !app->debugOn;
			debugPressed = true;
		} else
			debugPressed = false;


		Duration toSleep = {.microseconds = 1000000 / 200 - time_Reset(&lastFrame).microseconds};
		duration_Sleep(toSleep);
	}

	closegraph();
	app_DeleteApp(app);
	printf("%.6lf seconds has elapsed\n", duration_Seconds(time_Since(startup)));
	return 0;
}
