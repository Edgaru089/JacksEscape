
#include "Physics_Component.h"
#include "Entity.h"
#include "Types.h"
#include "util/tree.h"

#include <stdlib.h>


static const double EPS = 1e-6;

static inline double dabs(double x) {
	return x < 0 ? -x : x;
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
			if (delta > 0) {
				// Moves right, hits left edge
				double maxdelta = tohit.lefttop.x - mybox.lefttop.x - mybox.size.x;
				delta           = maxdelta - EPS;
			} else {
				// Moves left, hits right edge
				double maxdelta = tohit.lefttop.x - mybox.lefttop.x + tohit.size.x;
				delta           = maxdelta + EPS;
			}
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
			if (delta > 0) {
				// Moves down, hits top edge
				double maxdelta = tohit.lefttop.y - mybox.lefttop.y - mybox.size.y;
				delta           = maxdelta - EPS;
			} else {
				// Moves up, hits bottom edge
				double maxdelta = tohit.lefttop.y - mybox.lefttop.y + tohit.size.y;
				delta           = maxdelta + EPS;
			}
		}

		if (dabs(delta) < EPS)
			break;
	}

	if (dabs(delta) > EPS)
		e->position->position.y += delta;
}
