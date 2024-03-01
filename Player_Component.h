#pragma once

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
	int     jumpCount;        // Number of times the player has jumped since leaving ground
							  // (Initial jump does not count)
	bool onGround;            // If the player is on the ground?
	bool moveLeft, moveRight; // If the player is moving left/right?
} Component_Player;


// Player controller instance.
// Reads input from the app.
typedef struct {
	// The player in the world.
	// Control is paused if equals NULL.
	Component_Player *player;
} System_Player;


#ifdef __cplusplus
}
#endif
