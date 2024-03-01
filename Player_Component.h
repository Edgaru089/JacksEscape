#pragma once

#include "Types.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Entity Entity;


// Player controller component.
// Only one entity should have this.
typedef struct {
	Entity *super;

	int jumpCount;            // Number of times the player has jumped since leaving ground
							  // (Initial jump does not count)
	bool onGround;            // If the player is on the ground?
	bool moveLeft, moveRight; // If the player is moving left/right?
} Component_Player;


typedef struct _App App;

// Player controller instance.
// Reads input from the app.
typedef struct {
	App *super;
	// The player in the world.
	// Control is paused if equals NULL.
	Component_Player *player;
} System_Player;

System_Player *player_NewSystem(App *super);
void           player_DeleteSystem(System_Player *sys);

// Adds an entity.
void player_AddEntity(System_Player *sys, Entity *e);
// Deletes an entity.
void player_DeleteEntity(System_Player *sys, uintptr_t id);

// Called on every frame.
void player_Advance(System_Player *sys, Duration deltaTime);


#ifdef __cplusplus
}
#endif
