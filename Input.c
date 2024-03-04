
#include "Input.h"
#include "App.h"

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
	"Escape"};

void input_SetDefaultKeymap(System_Input *sys) {
	sys->systemKeymap[input_Key_Up]     = 'W';
	sys->systemKeymap[input_Key_Left]   = 'A';
	sys->systemKeymap[input_Key_Down]   = 'S';
	sys->systemKeymap[input_Key_Right]  = 'D';
	sys->systemKeymap[input_Key_Jump]   = VK_SPACE;
	sys->systemKeymap[input_Key_Attack] = 'J';
	sys->systemKeymap[input_Key_Spell]  = 'K';
	sys->systemKeymap[input_Key_Use]    = 'L';
	sys->systemKeymap[input_Key_Escape] = VK_ESCAPE;
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
			if (sys->keys[i] == JustReleased || sys->keys[i] == Released) {
				fprintf(stderr, "[input_Advance] Key %s pressed\n", input_KeyNames[i]);
				sys->keys[i] = JustPressed;
			} else
				sys->keys[i] = Pressed;
		} else {
			// Released
			if (sys->keys[i] == JustPressed || sys->keys[i] == Pressed)
				sys->keys[i] = JustReleased;
			else
				sys->keys[i] = Released;
		}
	}

	if (sys->keys[input_Key_Escape] == JustPressed) {
		fprintf(stderr, "[input_Advance] Let's quit now!\n");
		sys->super->wantQuit = true;
	}
}
