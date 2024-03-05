
#include "player.h"
#include "entity.h"
#include "app.h"
#include "input.h"

#include "types.h"
#include "util/assert.h"
#include <stdlib.h>


static inline double dabs(double x) {
	return x < 0.0 ? -x : x;
}


System_Player *player_NewSystem(App *super) {
	System_Player *sys = malloc(sizeof(System_Player));
	sys->super         = super;
	sys->player        = NULL;
	return sys;
}
void player_DeleteSystem(System_Player *sys) {
	free(sys);
}

static void _player_OnHit(Entity *me, Entity *other, Vec2 triedDelta, void *data) {
	ASSERT(me->player && "_player_OnHit called with ME as non-player");
	// Moving down
	if (triedDelta.y > 0)
		me->player->onGround = true;
}


void player_AddEntity(System_Player *sys, Entity *e) {
	if (e->player) {
		ASSERT(!sys->player && "Two player entities are added! Did you forget to DeleteEntity for player?");
		sys->player = e->player;

		ASSERT(e->hitbox && "Player does not have Hitbox component");
		e->hitbox->onHit = &_player_OnHit;
	}
}
void player_DeleteEntity(System_Player *sys, uintptr_t id) {
	if (sys->player && sys->player->super->id == id)
		sys->player = NULL;
}


static double   walkSpeed = 300.0, jumpSpeed = 800.0, dashSpeed = 1500.0;
static int      airjumpCount = 1, airdashCount = 1;
static Duration dashLength = {.microseconds = 150000}, dashCooldown = {.microseconds = 400000};

void player_Advance(System_Player *sys, Duration deltaTime) {
	if (!sys->player)
		return;
	// The bulk of player logic right here
	System_Input *input = sys->super->input;

	if (sys->player->faceDirection != 1 && sys->player->faceDirection != -1)
		sys->player->faceDirection = 1; // Face right by default


	double targetVecX = 0.0;
	// Move left/right
	if (input_IsPressed(input->keys[input_Key_Left])) {
		sys->player->faceDirection = -1;
		targetVecX += -walkSpeed;
	}
	if (input_IsPressed(input->keys[input_Key_Right])) {
		sys->player->faceDirection = 1;
		targetVecX += walkSpeed;
	}
	sys->player->super->position->velocity.x = targetVecX;


	// Jump
	if (sys->player->onGround)
		sys->player->jumpCount = 0;
	if (sys->super->input->keys[input_Key_Jump] == JustPressed ||
		(sys->super->input->keys[input_Key_Jump] == Pressed && dabs(sys->player->super->position->velocity.y) < 5)) {
		if (sys->player->onGround || sys->player->jumpCount < airjumpCount) {
			sys->player->storedSpeedY = -jumpSpeed;
			if (!sys->player->onGround) // Took the second clause, airjumped
				sys->player->jumpCount++;
		}
	}

	// Dash
	if (sys->player->onGround)
		sys->player->dashCount = 0;
	if (input_IsPressed(input->keys[input_Key_Dash]) &&
		(sys->player->onGround || sys->player->dashCount < airdashCount) &&
		time_Since(sys->player->lastDash).microseconds > dashCooldown.microseconds) {
		sys->player->lastDash = time_Now();
		if (!sys->player->onGround)
			sys->player->dashCount++;
	}
	// Am I dashing right now?
	if (time_Since(sys->player->lastDash).microseconds < dashLength.microseconds) {
		sys->player->super->position->velocity.x += sys->player->faceDirection * dashSpeed;
		sys->player->super->position->velocity.y = 0;
	} else { // Release the stored Y speed
		sys->player->super->position->velocity.y += sys->player->storedSpeedY;
		sys->player->storedSpeedY = 0;
	}


	// Check OnGround again
	if (dabs(sys->player->super->position->velocity.y) > EPS)
		sys->player->onGround = false;
}
