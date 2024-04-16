#pragma once

#include "types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


#define TEXTHEIGHT 24


// Draws text at the given on-screen position.
//
// unlike outtext(), this function takes consideration
// for newlines.
void render_DrawText(int x, int y, const char *str);


// Fill modes.
typedef struct {
	// Most often it's:
	//     R2_COPYPEN (Output = Pen color)
	//     R2_BLACK   (Output = Black)
	//     R2_WHITE   (Output = White)
	//     R2_NOT     (Output = NOT Screen color)
	//
	// https://docs.easyx.cn/zh-cn/setrop2
	int rop2;

	// Style & hatch passed into setfillstyle()
	//
	// https://docs.easyx.cn/zh-cn/setfillstyle
	int  style;
	long hatch;

	// Rotates in /-\| every this amount of time, if not 0
	// Overrides style & hatch
	Duration rotate;

	// Dissolve the entire screen in an animation in this
	// amount of time. Overrides rotate, style & hatch
	Duration dissolve;
	bool     fadein; // Fade Into being full for true, fade out of otherwise
} FillMode;

// Default fill mode.
//     rop2 = R2_COPYPEN,
//     style = BS_SOLID,
//     others = 0
extern const FillMode render_ModeDefault;
// Fill mode that inverses the screen.
//     rop2 = R2_NOT,
//     style = BS_SOLID,
//     others = 0
extern const FillMode render_ModeInverse;
// Fill mode that rotates.
extern const FillMode render_ModeRotate;

typedef struct _App App;

// Calls the required setfillstyle/setrop2 calls for the struct.
void render_SetModes(FillMode mode, TimePoint since);

// Fills the entire screen
void render_FillScreen();

// Fills a rectangle in world coordinates
void render_FillRectW(App *app, Box2 rect);
// Fills a circle in world coordinates
void render_FillCircleW(App *app, Vec2 center, double radius);


#ifdef __cplusplus
}
#endif
