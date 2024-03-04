
#include "Player_Component.h"
#include "Entity.h"
#include "App.h"
#include "Input.h"

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


static double walkSpeed = 200.0, jumpSpeed = 500.0;
static int    airjumpCount = 1;

void player_Advance(System_Player *sys, Duration deltaTime) {
	if (!sys->player)
		return;
	// The bulk of player logic right here
	System_Input *input = sys->super->input;

	double targetVecX = 0.0;
	// Move left/right
	if (input_IsPressed(input->keys[input_Key_Left]))
		targetVecX += -walkSpeed;
	if (input_IsPressed(input->keys[input_Key_Right]))
		targetVecX += walkSpeed;

	sys->player->super->position->velocity.x = targetVecX;

	if (sys->player->onGround)
		sys->player->jumpCount = 0;

	// Jump
	if (sys->super->input->keys[input_Key_Jump] == JustPressed) {
		if (sys->player->onGround || sys->player->jumpCount < airjumpCount)
			sys->player->super->position->velocity.y = -jumpSpeed;
		if (!sys->player->onGround) // Took the second clause, airjumped
			sys->player->jumpCount++;
	}

	// Check OnGround again
	if (dabs(sys->player->super->position->velocity.y) > EPS)
		sys->player->onGround = false;
}
