#pragma once

#include "types.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _App App;


// GameTime starts counting at 100 seconds.
#define GAMETIME_EPOCH ((int64_t)(100 * 1000 * 1000))


// GameTime keeps track of running (or paused) time of the game.
typedef struct {
	App    *super;
	int64_t now; // Microseconds since object creation, adding GAMETIME_EPOCH.
} System_GameTime;


System_GameTime *gametime_NewSystem(App *super);
void             gametime_DeleteSystem(System_GameTime *sys);

// Advance pushes the clock forwards by DeltaTime.
void gametime_Advance(System_GameTime *sys, Duration deltaTime);


// time_XXX functions, ported to GameTime.
TimePoint gametime_Now(System_GameTime *sys);
Duration  gametime_Since(System_GameTime *sys, TimePoint prev);
Duration  gametime_Reset(System_GameTime *sys, TimePoint *prev);


#ifdef __cplusplus
}
#endif
