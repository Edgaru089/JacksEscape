#pragma once

#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


// Types of input the player might want
typedef enum {
	input_Key_Left,
	input_Key_Right,
	input_Key_Up,
	input_Key_Down,
	input_Key_Jump,
	input_Key_Attack,
	input_Key_Spell,
	input_Key_Use,
	input_Key_Escape,
	input_Key_Count
} input_Key;

// Names for input_Key
extern const char *input_KeyNames[input_Key_Count];


// States a key might in
typedef enum {
	Released,
	JustReleased,
	Pressed,
	JustPressed
} input_KeyState;

static inline bool input_IsPressed(input_KeyState state) { return state == Pressed || state == JustPressed; }
static inline bool input_IsReleased(input_KeyState state) { return state == Released || state == JustReleased; }


typedef struct _App App;

typedef struct {
	App *super;

	input_KeyState keys[input_Key_Count];         // States of keys
	unsigned int   systemKeymap[input_Key_Count]; // Which system key (VK code) this function uses
												  // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
} System_Input;


// Creates a new input system.
// Uses a default keymap
System_Input *input_NewSystem(App *super);
void          input_DeleteSystem(System_Input *sys);

// Sets the keymaps to default.
void input_SetDefaultKeymap(System_Input *sys);

// Update key states
void input_Advance(System_Input *sys);


#ifdef __cplusplus
}
#endif
