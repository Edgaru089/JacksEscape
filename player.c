
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


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


static inline double dabs(double x) {
	return x < 0.0 ? -x : x;
}


System_Player *player_NewSystem(App *super) {
	System_Player *sys = malloc(sizeof(System_Player));
	sys->super         = super;
	sys->player        = NULL;
	sys->cutoff        = 5000.0;
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
static int      airjumpCount = 1, airdashCount = 1, airjumpParticleCount = 10;
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
	static Duration  emitCooldown = {.microseconds = 80000};
	static TimePoint lastEmit     = {.microseconds = 0};
	if (time_Since(p->lastDash).microseconds < dashLength.microseconds && dabs(p->super->position->velocity.x) > EPS) {
		// When dashing
		static TimePoint lastDashEmit = {.microseconds = 0};
		static Duration  cooldown     = {.microseconds = 4000};
		if (time_Since(lastDashEmit).microseconds > cooldown.microseconds) {
			lastDashEmit = time_Now();
			Vec2 to_pos  = vec2_Add(p->super->position->position, vec2(0, -p->super->hitbox->box.size.y / 2.0));
			particle_Emit(
				sys->super->particle,
				vec2_Add(vec2_Random(-5, 5, -30, 30), to_pos),
				vec2(rand_DoubleRange(650, 700) * -p->faceDirection, rand_DoubleRange(-100, 100)),
				7, rand_DoubleRange(14, 20), rand_DoubleRange(32, 40),
				duration_FromSeconds(0), &render_ModeRotate);
		}
		lastEmit = time_Now();
		lastEmit.microseconds -= 74000;
	} else {
		// When not dashing
		if (time_Since(lastEmit).microseconds > emitCooldown.microseconds) {
			lastEmit = time_Now();
			lastEmit.microseconds += duration_FromSeconds(0.05 * rand_Double01()).microseconds;
			Vec2 to_pos = vec2_Add(p->super->position->position, vec2(0, -p->super->hitbox->box.size.y));
			particle_Emit(
				sys->super->particle,
				vec2_Add(vec2_Random(-10, 10, -10, 10), to_pos),
				vec2_Add(
					vec2(0, rand_DoubleRange(-200, -240)),
					vec2_Scale(p->super->position->velocity, rand_DoubleRange(0.75, 1.0))),
				2, round(rand_DoubleRange(12, 16)), 20,
				duration_FromSeconds(0), &render_ModeDefault);
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
			// Particles fly up if on ground, down if airjumped
			for (int i = 0; i < airjumpParticleCount; i++)
				particle_Emit(
					sys->super->particle,
					vec2_Add(vec2_Random(-20, 20, -5, 5), p->super->position->position),
					(p->onGround) ? vec2(rand_DoubleRange(-50, 50), -100) : vec2(0, 200), 2, rand_DoubleRange(8, 12), 20,
					duration_FromSeconds(0), &render_ModeRotate);
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
		if (dabs(p->storedSpeedY) > EPS) {
			if (p->super->position->velocity.y < 0 && p->super->position->velocity.y > p->storedSpeedY)
				p->super->position->velocity.y += p->storedSpeedY;
			else
				p->super->position->velocity.y = p->storedSpeedY;
			p->storedSpeedY = 0;
		}
	}


	// Respawn if fallen out of the world
	if (p->super->position->position.y > sys->cutoff)
		player_HazardHarm(sys);


	// Check OnGround again
	if (dabs(sys->player->super->position->velocity.y) > EPS)
		sys->player->onGround = false;
}


static int harmed_particle_count = 20;

void player_HazardHarm(System_Player *sys) {
	if (!sys->player)
		return;

	// Emit some particles
	for (int i = 0; i < harmed_particle_count; i++) {
		double speed_linear = rand_DoubleRange(150, 400);
		double angle        = rand_Double01() * 2.0 * M_PI;
		particle_Emit(
			sys->super->particle,
			sys->player->super->position->position,
			vec2(speed_linear * cos(angle), speed_linear * sin(angle)),
			rand_DoubleRange(2, 3),
			rand_DoubleRange(5, 28),
			rand_DoubleRange(20, 32),
			duration_FromSeconds(0),
			&render_ModeDefault);
	}

	sys->player->storedSpeedY              = 0;
	sys->player->super->position->velocity = vec2(0, 0);
	sys->player->super->position->position = sys->player->hazardRespawn;
	sys->player->super->position->position.y -= EPS; // Stay loose!
}
