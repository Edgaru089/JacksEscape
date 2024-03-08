
#include "camera.h"
#include "types.h"
#include "entity.h"
#include "util/assert.h"


static inline double dabs(double x) { return x < 0.0 ? -x : x; }
static inline double dmin(double x, double y) { return x < y ? x : y; }


System_Camera *camera_NewSystem(App *super) {
	System_Camera *sys = zero_malloc(sizeof(System_Camera));
	sys->super         = super;

	Box2 default_screen = {
		.lefttop = {.x = 0.0, .y = 0.0},
		.size    = {.x = 1280, .y = 720}};
	sys->cam = sys->screen = default_screen;
	return sys;
}
void camera_DeleteSystem(System_Camera *sys) {
	free(sys);
}


void camera_AddEntity(System_Camera *sys, Entity *e) {
	if (e->player) {
		ASSERT(!sys->player && "Two player entities are added! Did you forget to DeleteEntity for player?");
		sys->player = e->player;
	}
}
void camera_DeleteEntity(System_Camera *sys, uintptr_t id) {
	if (sys->player && sys->player->super->id == id)
		sys->player = NULL;
}


void camera_Advance(System_Camera *sys, Duration deltaTime) {
	Vec2 tocenter;
	if (sys->target.size.x > EPS)
		tocenter = box2_Center(sys->target);
	else if (sys->player)
		tocenter = sys->player->super->position->position;
	else
		return;

	double speed;
	if (dabs(sys->speed) < EPS)
		speed = 4.0;
	else
		speed = sys->speed;

	Vec2 oldcenter = box2_Center(sys->cam), oldsize = sys->cam.size;
	Vec2 newcenter, newsize;
	Vec2 diff = vec2_Minus(tocenter, oldcenter);
	if (dabs(diff.x) < EPS && dabs(diff.y) < EPS)
		newcenter = tocenter;
	else {
		Vec2 xdiff = vec2_Scale(diff, dmin(speed * duration_Seconds(deltaTime), 1.0));
		newcenter  = vec2_Add(oldcenter, xdiff);
	}

	// TODO size scaling
	newsize  = oldsize;
	sys->cam = box2_FromCenter(newcenter, newsize);
}


inline static Vec2 _camera_TransformSize(System_Camera *sys, Vec2 worldSize) {
	Vec2 result = {
		.x = worldSize.x / sys->cam.size.x * sys->screen.size.x,
		.y = worldSize.y / sys->cam.size.y * sys->screen.size.y};
	return result;
}

Vec2 camera_TransformVec2(System_Camera *sys, Vec2 world) {
	Vec2 relative = vec2_Minus(world, sys->cam.lefttop);
	return vec2_Add(sys->screen.lefttop, _camera_TransformSize(sys, relative));
}
Box2 camera_TransformBox2(System_Camera *sys, Box2 world) {
	Box2 result = {
		.lefttop = camera_TransformVec2(sys, world.lefttop),
		.size    = _camera_TransformSize(sys, world.size)};
	return result;
}
