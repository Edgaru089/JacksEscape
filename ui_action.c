
#include "ui.h"


void ui_Action_PopState(System_UI *sys, ui_Part *part, uintptr_t data) {
	ui_PopState(sys);
}
void ui_Action_PushState(System_UI *sys, ui_Part *part, uintptr_t data) {
	ui_PushState(sys, (ui_State)data);
}
