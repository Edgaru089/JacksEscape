#pragma once

#include "Types.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct Entity;
extern double Physics_Gravity;


typedef struct {
	Entity *super;
	double x, y;
	double vecX, vecY;
} Component_Position;


// Box is relative to Position if exists
// if not, Box is absolute
//
// Moving hitboxes only hits fixed hitboxes
typedef struct {
	Entity *super;
	Box2 box;
	bool fixed;
} Component_Hitbox;


#ifdef __cplusplus
}
#endif
