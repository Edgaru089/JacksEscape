
#include "player.h"
#include "entity.h"
#include "app.h"
#include "input.h"
#include "particle.h"

#include "render_util.h"
#include "types.h"
#include "util/assert.h"
#include "util/rand.h"
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
	System_Input     *input = sys->super->input;
	Component_Player *p     = sys->player;

	if (p->faceDirection != 1 && p->faceDirection != -1)
		p->faceDirection = 1; // Face right by default

	// Particles
	static Duration  emitCooldown = {.microseconds = 150000};
	static TimePoint lastEmit     = {};
	if (time_Since(lastEmit).microseconds > emitCooldown.microseconds) {
		lastEmit    = time_Now();
		Vec2 to_pos = vec2_Add(p->super->position->position, vec2(0, -p->super->hitbox->box.size.y));
		particle_Emit(
			sys->super->particle,
			vec2_Add(vec2_Random(-20, 20, -10, 10), to_pos),
			vec2(0, -100), 2, 6, 6,
			duration_FromSeconds(0), &render_ModeDefault);
	}
	// Particles when dashing
	if (time_Since(p->lastDash).microseconds < dashLength.microseconds && dabs(p->super->position->velocity.x) > EPS) {
		static TimePoint lastDashEmit = {};
		static Duration  cooldown     = {.microseconds = 10000};
		if (time_Since(lastDashEmit).microseconds > cooldown.microseconds) {
			lastDashEmit = time_Now();
			Vec2 to_pos  = vec2_Add(p->super->position->position, vec2(0, -p->super->hitbox->box.size.y / 2.0));
			particle_Emit(
				sys->super->particle,
				vec2_Add(vec2_Random(-5, 5, -30, 30), to_pos),
				vec2(rand_DoubleRange(650, 700) * -p->faceDirection, rand_DoubleRange(-100, 100)),
				7, rand_DoubleRange(10, 16), 3,
				duration_FromSeconds(rand_DoubleRange(1.5, 2.0)), &render_ModeInverse);
		}
	}


	double targetVecX = 0.0;
	// Move left/right
	if (input_IsPressed(input->keys[input_Key_Left])) {
		p->faceDirection = -1;
		targetVecX += -walkSpeed;
	}
	if (input_IsPressed(input->keys[input_Key_Right])) {
		p->faceDirection = 1;
		targetVecX += walkSpeed;
	}
	p->super->position->velocity.x = targetVecX;


	// Jump
	if (p->onGround)
		p->jumpCount = 0;
	if (input->keys[input_Key_Jump] == JustPressed ||
		(input->keys[input_Key_Jump] == Pressed && dabs(p->super->position->velocity.y) < 5)) {
		if (p->onGround || p->jumpCount < airjumpCount) {
			p->storedSpeedY = -jumpSpeed;
			if (!p->onGround) // Took the second clause, airjumped
				p->jumpCount++;
		}
	}

	// Dash
	if (p->onGround)
		p->dashCount = 0;
	if (input_IsPressed(input->keys[input_Key_Dash]) &&
		(p->onGround || p->dashCount < airdashCount) &&
		time_Since(p->lastDash).microseconds > dashCooldown.microseconds) {
		p->lastDash = time_Now();
		if (!p->onGround)
			p->dashCount++;
	}
	// Am I dashing right now?
	if (time_Since(p->lastDash).microseconds < dashLength.microseconds) {
		p->super->position->velocity.x += p->faceDirection * dashSpeed;
		p->super->position->velocity.y = 0;
	} else { // Release the stored Y speed
		p->super->position->velocity.y += p->storedSpeedY;
		p->storedSpeedY = 0;
	}


	// Check OnGround again
	if (dabs(sys->player->super->position->velocity.y) > EPS)
		sys->player->onGround = false;
}
