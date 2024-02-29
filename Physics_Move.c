
#include "Component_Physics.h"
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
	if (dabs(delta) < EPS)
		return;

	for (tree_Node *i = tree_FirstNode(sys->hit);
		 i != NULL;
		 i = tree_Node_Next(i)) {
		Component_Hitbox *tohit = *((Component_Hitbox **)i->data);
		if (!tohit->fixed)
			continue;

		if (box2_Intersects(tohit->box, box2_OffsetX(e->hitbox->box, delta), NULL)) {
			if (delta > 0) {
				// Moves right, hits left edge
				double maxdelta = tohit->box.lefttop.x - e->hitbox->box.lefttop.x - e->hitbox->box.size.x;
				delta           = maxdelta - EPS;
			} else {
				// Moves left, hits right edge
				double maxdelta = tohit->box.lefttop.x - e->hitbox->box.lefttop.x + tohit->box.size.x;
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
	if (dabs(delta) < EPS)
		return;

	for (tree_Node *i = tree_FirstNode(sys->hit);
		 i != NULL;
		 i = tree_Node_Next(i)) {
		Component_Hitbox *tohit = *((Component_Hitbox **)i->data);
		if (!tohit->fixed)
			continue;

		if (box2_Intersects(tohit->box, box2_OffsetY(e->hitbox->box, delta), NULL)) {
			if (delta > 0) {
				// Moves down, hits top edge
				double maxdelta = tohit->box.lefttop.y - e->hitbox->box.lefttop.y - e->hitbox->box.size.y;
				delta           = maxdelta - EPS;
			} else {
				// Moves up, hits bottom edge
				double maxdelta = tohit->box.lefttop.y - e->hitbox->box.lefttop.y + tohit->box.size.y;
				delta           = maxdelta + EPS;
			}
		}

		if (dabs(delta) < EPS)
			break;
	}

	if (dabs(delta) > EPS)
		e->position->position.y += delta;
}
