#pragma once

#include "player.h"
#include "types.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct _App App;

typedef struct {
	App              *super;
	Component_Player *player;

	Box2   cam, target; // Current & target camera
	double speed;       // Fraction of distance between cam & target to be covered in 1 sec
} System_Camera;

System_Camera *camera_NewSystem(App *super);
void           camera_DeleteSystem(System_Camera *sys);

void camera_Advance(System_Camera *sys, Duration deltaTime);


#ifdef __cplusplus
}
#endif
