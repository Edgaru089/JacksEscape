#pragma once

#include "types.h"
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

	int       faceDirection; // +1 for right, -1 for left
	TimePoint lastDash;
	int       dashCount; // Number of dashes the player did since in air
	int       jumpCount; // Number of times the player has jumped since leaving ground
						 // (Initial jump does not count)

	double storedSpeedY;        // Speed stored in the middle of a dash
	bool   onGround;            // If the player is on the ground?
	bool   moveLeft, moveRight; // If the player is moving left/right?

	Vec2 hazardRespawn; // Where the last hazard respawn is
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

// Harm the player.
// The player is hit backwards, releasing a puff of smoke.
//
// To be implemented
// void player_Harm(System_Player *sys, Vec2 source);

// Hazard harm the player.
// The player is teleported to the hazard respawn point.
void player_HazardHarm(System_Player *sys);


#ifdef __cplusplus
}
#endif
