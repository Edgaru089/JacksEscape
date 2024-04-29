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
	ui_InterMission,
	ui_Leaderboards,
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
	void (*free)(System_UI *sys, struct _ui_Part *part, uintptr_t user);                       // called when the element is to be deleted, dont free part!
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

void     ui_PushState(System_UI *sys, ui_State push_state);
void     ui_PopState(System_UI *sys);
ui_State ui_CurrentState(System_UI *sys);


// Called when the level is finished
// If next_level is NULL, there are no next level and the
// button returns to title screen
void ui_EnterIntermission(System_UI *sys, const char *next_level);


#define UI_PADDING    (16)
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


typedef struct {
	char *label; // Allocated & copied, NULL means no label
	int   align; // <0: left aligned; ==0: center aligned; >0: right aligned
} ui_Label;
ui_Part ui_Label_New(Box2 box, const char *label, int alignment);
void    ui_Label_SetLabel(ui_Part *part, const char *label);

// Creates a new box fill with the given color
ui_Part ui_Fill_New(Box2 box, uint32_t color);


// Box organizer
typedef struct {
	vector_Vector *rows;        // vector of vector_Vector* of Box2
	vector_Vector *row_counts;  // vector of Ints
	double         width;       // Total width, excluding external padding
	double         line_height; // Line height
	Vec2           position;    // Position of the entire bounding box on screen
	Vec2           pivot;       // Pivot or Origin of the position, [0, 1]
} ui_BoxBuilder;

#define UI_BOXBUILDER_DEFAULT_LINEHEIGHT (48)
#define UI_BOXBUILDER_DEFAULT_WIDTH      (400)
#define UI_BOXBUILDER_DEFAULT_PIVOT_X    (0.5)
#define UI_BOXBUILDER_DEFAULT_PIVOT_Y    (0.0)

ui_BoxBuilder *ui_BoxBuilder_New();
void           ui_BoxBuilder_Delete(ui_BoxBuilder *builder);

// Set that this row should have this many columns
void ui_BoxBuilder_SetCount(ui_BoxBuilder *builder, int row, int num_cols);

// Assemble everything
void ui_BoxBuilder_Assemble(ui_BoxBuilder *builder);

// Every number starts from 1
Box2 ui_BoxBuilder_At(ui_BoxBuilder *builder, int row, int col);


#ifdef __cplusplus
}
#endif
