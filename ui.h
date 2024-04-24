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
	ui_SceneSelect,
	ui_Options,
	ui_StateCount // Keep at last
} ui_State;

extern const char *ui_StateTitle[ui_StateCount];


// Forward declarations
typedef struct _System_UI System_UI;

// a Part of the UI.
typedef struct _ui_Part {
	Box2      box;                                                                             // bounding box of the UI element
	float     progress;                                                                        // hover process, from 0 to 1
	bool      hovered;                                                                         // Whether the part is hovered
	uintptr_t user;                                                                            // user data
	void (*draw)(System_UI *sys, struct _ui_Part *part, uintptr_t user);                       // called when the UI is to be drawn
	void (*update)(System_UI *sys, struct _ui_Part *part, uintptr_t user, Duration deltatime); // called when the element is to be updated
	void (*free)(System_UI *sys, struct _ui_Part *part, uintptr_t user);                       // called when the element is to be deleted
} ui_Part;

// UI system with its own state machine.
typedef struct _System_UI {
	App           *super;
	vector_Vector *parts[ui_StateCount]; // A vector of ui_Parts
	vector_Vector *state;                // A stack of ui_States

	Box2 bg, bg_target;
} System_UI;

System_UI *ui_NewSystem(App *super);
void       ui_DeleteSystem(System_UI *sys);

// Clears and refills every menu with its widgets
void ui_RebuildUI(System_UI *sys);

void ui_PushPart(System_UI *sys, ui_State layer, ui_Part part);

void ui_Advance(System_UI *sys, Duration deltaTime);
void ui_Render(System_UI *sys);

void ui_PushState(System_UI *sys, ui_State push_state);
void ui_PopState(System_UI *sys);

ui_State ui_CurrentState(System_UI *sys);


#define UI_PADDING    (20)
#define UI_BACKGROUND (0)

#define UI_BACKGROUND_FADE_MOVE_SPEED (20.0)
#define UI_BUTTON_HOVER_SPEED         (5.0)

#define UI_BUTTON_BACKGROUND (RGB(50, 50, 50))
#define UI_BUTTON_HOVERED    (RGB(75, 75, 75))
#define UI_BUTTON_CLICKED    (RGB(90, 90, 90))

typedef void (*ui_ActionCallback)(System_UI *sys, ui_Part *part, uintptr_t data);

// An Action to pop the UI state stack once
void ui_Action_PopState(System_UI *sys, ui_Part *part, uintptr_t data);
// An Action to push the state, casted directly from data
void ui_Action_PushState(System_UI *sys, ui_Part *part, uintptr_t data);


// Allocated & passed in as user data for ui part
typedef struct _ui_Button {
	ui_ActionCallback callback;
	uintptr_t         callback_data;

	uint32_t bg, bg_hover, clicked;
	char    *label; // Allocated & copied
	bool     left_aligned;
} ui_Button;

ui_Part ui_Button_New(Box2 box, const char *label, ui_ActionCallback callback, uintptr_t data);
ui_Part ui_Button_NewLeftAligned(Box2 box, const char *label, ui_ActionCallback callback, uintptr_t data);
// ui_Part ui_Button_NewColored(Box2 box, const char *label, uint32_t bg, uint32_t hover, uint32_t clicked, ui_ActionCallback callback, uintptr_t data);


#ifdef __cplusplus
}
#endif
