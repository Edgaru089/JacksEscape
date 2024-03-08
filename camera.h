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

	Box2   screen;      // Screen box, e.g. 1280x720 origin (0,0)
	Box2   cam, target; // Current & target camera
	double speed;       // Fraction of distance between cam & target to be covered in 1 sec
} System_Camera;

System_Camera *camera_NewSystem(App *super);
void           camera_DeleteSystem(System_Camera *sys);

void camera_AddEntity(System_Camera *sys, Entity *e);
void camera_DeleteEntity(System_Camera *sys, uintptr_t id);

void camera_Advance(System_Camera *sys, Duration deltaTime);

Vec2 camera_TransformVec2(System_Camera *sys, Vec2 world);
Box2 camera_TransformBox2(System_Camera *sys, Box2 world);


#ifdef __cplusplus
}
#endif
