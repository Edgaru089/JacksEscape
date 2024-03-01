
#include "Input.h"
#include "App.h"

#include <stdlib.h>
#include <windows.h>


void input_SetDefaultKeymap(System_Input *sys) {
	sys->systemKeymap[input_Key_Up]     = 'W';
	sys->systemKeymap[input_Key_Left]   = 'A';
	sys->systemKeymap[input_Key_Down]   = 'S';
	sys->systemKeymap[input_Key_Right]  = 'D';
	sys->systemKeymap[input_Key_Jump]   = VK_SPACE;
	sys->systemKeymap[input_Key_Attack] = 'J';
	sys->systemKeymap[input_Key_Spell]  = 'K';
	sys->systemKeymap[input_Key_Use]    = 'L';
}

System_Input *input_NewSystem(App *super) {
	System_Input *sys = malloc(sizeof(System_Input));
	sys->super        = super;
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
}
