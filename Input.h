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
	input_Key_Count
} input_Key;

// States a key might in
typedef enum {
	Pressed,
	JustPressed,
	Released,
	JustReleased
} input_KeyState;

typedef struct {
	input_KeyState keys[input_Key_Count]; // States of keys
} System_Input;


#ifdef __cplusplus
}
#endif
