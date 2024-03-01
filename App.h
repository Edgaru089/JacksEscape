#pragma once

#include "Entity.h"
#include "Physics_Component.h"
#include "Player_Component.h"
#include "Input.h"


typedef struct _App {
	System_Physics *physics;
	System_Player  *player;
	System_Input   *input;
} App;
