
#include "input.h"
#include "app.h"
#include "ui.h"

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>


const char *input_KeyNames[input_Key_Count] = {
	"Left",
	"Right",
	"Up",
	"Down",
	"Jump",
	"Attack",
	"Spell",
	"Use",
	"Dash",
	"Escape",
	"Left Mouse Button",
	"Right Mouse Button"};

void input_SetDefaultKeymap(System_Input *sys) {
	sys->systemKeymap[input_Key_Up]         = 'W';
	sys->systemKeymap[input_Key_Left]       = 'A';
	sys->systemKeymap[input_Key_Down]       = 'S';
	sys->systemKeymap[input_Key_Right]      = 'D';
	sys->systemKeymap[input_Key_Jump]       = VK_SPACE;
	sys->systemKeymap[input_Key_Attack]     = 'J';
	sys->systemKeymap[input_Key_Spell]      = 'K';
	sys->systemKeymap[input_Key_Use]        = 'L';
	sys->systemKeymap[input_Key_Dash]       = VK_OEM_1; // The ;: key on the US keyboard
	sys->systemKeymap[input_Key_Escape]     = VK_ESCAPE;
	sys->systemKeymap[input_Key_LeftMouse]  = VK_LBUTTON;
	sys->systemKeymap[input_Key_RightMouse] = VK_RBUTTON;
}

System_Input *input_NewSystem(App *super) {
	System_Input *sys = malloc(sizeof(System_Input));
	memset(sys, 0, sizeof(System_Input));
	sys->super = super;
	input_SetDefaultKeymap(sys);
	return sys;
}

void input_DeleteSystem(System_Input *sys) {
	free(sys);
}


void input_Advance(System_Input *sys) {
	for (int i = 0; i < input_Key_Count; i++) {
		// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getasynckeystate
		// Checks the most significiant bit (of the SHORT returned)
		if ((GetAsyncKeyState(sys->systemKeymap[i]) & 0x8000) != 0) {
			// Pressed
			if (sys->keys[i] == JustReleased || sys->keys[i] == Released)
				sys->keys[i] = JustPressed;
			else
				sys->keys[i] = Pressed;
		} else {
			// Released
			if (sys->keys[i] == JustPressed || sys->keys[i] == Pressed)
				sys->keys[i] = JustReleased;
			else
				sys->keys[i] = Released;
		}
	}

	if (input_IsPressed(sys->keys[input_Key_Spell])) {
		sys->super->timescale = 0.25;
	} else {
		sys->super->timescale = 1.0;
	}

	if (sys->keys[input_Key_Escape] == JustPressed) {
		if (ui_CurrentState(sys->super->ui) == ui_Running) {
			// Pause
			ui_PushState(sys->super->ui, ui_Paused);
			sys->super->paused = true;
		} else if (ui_CurrentState(sys->super->ui) == ui_Paused) {
			// Unpause
			ui_PopState(sys->super->ui);
			sys->super->paused = false;
		}
	}
}


Vec2 input_MousePosition(System_Input *sys) {
	POINT point;
	GetCursorPos(&point);
	ScreenToClient((HWND)sys->super->window, &point);
	return vec2(point.x, point.y);
}
