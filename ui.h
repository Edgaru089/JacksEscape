#pragma once

#include "types.h"
#include "util/vector.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _App App;

typedef enum {
	ui_Running,
	ui_Paused,
	ui_TitleMenu,
	ui_StateCount // Keep at last
} ui_States;


// Forward declarations
typedef struct _ui_Button ui_Button;
typedef struct _System_UI System_UI;

// a Part of the UI.
typedef struct {
	void *user;                               // user data
	void (*draw)(System_UI *sys, void *user); // called when the UI is to be drawn
} ui_Part;

// UI system with its own state machine.
typedef struct _System_UI {
	App           *super;
	vector_Vector *parts[ui_StateCount];
} System_UI;


#ifdef __cplusplus
}
#endif
