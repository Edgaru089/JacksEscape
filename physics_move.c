
#include "physics.h"
#include "entity.h"
#include "types.h"
#include "util/tree.h"

#include <stdlib.h>
#include <stdio.h>


static inline double dabs(double x) {
	return x < 0 ? -x : x;
}

static inline void call_hithandler(Entity *me, Entity *other, Vec2 triedDelta, void *data) {
	if (me->hitbox->onHit)
		me->hitbox->onHit(me, other, triedDelta, data);
}
static inline void call_hitby(Entity *me, Entity *other, Vec2 triedDelta, void *data) {
	if (me->hitbox->onHitBy)
		me->hitbox->onHitBy(me, other, triedDelta, data);
}


void _physics_MoveX(System_Physics *sys, Entity *e, Duration deltaTime) {
	double delta = e->position->velocity.x * duration_Seconds(deltaTime);
	Box2   mybox = physics_HitboxAbsolute(e->hitbox);
	if (dabs(delta) < EPS)
		return;

	for (tree_Node *i = tree_FirstNode(sys->hit);
		 i != NULL;
		 i = tree_Node_Next(i)) {
		Component_Hitbox *tohit_comp = *((Component_Hitbox **)i->data);
		Box2              tohit      = physics_HitboxAbsolute(tohit_comp);
		if (!tohit_comp->fixed)
			continue;

		if (box2_Intersects(tohit, box2_OffsetX(mybox, delta), NULL)) {
			call_hithandler(e, tohit_comp->super, vec2(delta, 0), e->hitbox->onHitData);
			call_hitby(tohit_comp->super, e, vec2(delta, 0), tohit_comp->onHitData);
			if (delta > 0) {
				// Moves right, hits left edge
				double maxdelta = tohit.lefttop.x - mybox.lefttop.x - mybox.size.x;
				delta           = maxdelta - EPS;
			} else {
				// Moves left, hits right edge
				double maxdelta = tohit.lefttop.x - mybox.lefttop.x + tohit.size.x;
				delta           = maxdelta + EPS;
			}
			e->position->velocity.x = 0;
		}

		if (dabs(delta) < EPS)
			break;
	}

	if (dabs(delta) > EPS)
		e->position->position.x += delta;
}

void _physics_MoveY(System_Physics *sys, Entity *e, Duration deltaTime) {
	double delta = e->position->velocity.y * duration_Seconds(deltaTime);
	Box2   mybox = physics_HitboxAbsolute(e->hitbox);
	if (dabs(delta) < EPS)
		return;

	for (tree_Node *i = tree_FirstNode(sys->hit);
		 i != NULL;
		 i = tree_Node_Next(i)) {
		Component_Hitbox *tohit_comp = *((Component_Hitbox **)i->data);
		Box2              tohit      = physics_HitboxAbsolute(tohit_comp);
		if (!tohit_comp->fixed)
			continue;

		if (box2_Intersects(tohit, box2_OffsetY(mybox, delta), NULL)) {
			call_hithandler(e, tohit_comp->super, vec2(0, delta), e->hitbox->onHitData);
			call_hitby(tohit_comp->super, e, vec2(0, delta), tohit_comp->onHitData);
			if (delta > 0) {
				// Moves down, hits top edge
				double maxdelta = tohit.lefttop.y - mybox.lefttop.y - mybox.size.y;
				delta           = maxdelta - EPS;
			} else {
				// Moves up, hits bottom edge
				double maxdelta = tohit.lefttop.y - mybox.lefttop.y + tohit.size.y;
				delta           = maxdelta + EPS;
			}
			e->position->velocity.y = 0;
		}

		if (dabs(delta) < EPS)
			break;
	}

	if (dabs(delta) > EPS)
		e->position->position.y += delta;
}
