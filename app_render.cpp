
#include "app.h"
#include "physics.h"
#include "easyx.h"
#include "util/tree.h"
#include <math.h>
#include <graphics.h>


extern "C" {


void app_Render(App *app) {
	for (tree_Node *i = tree_FirstNode(app->entity->entities);
		 i != NULL;
		 i = tree_Node_Next(i)) {
		Entity *e = (Entity *)(i->data);
		if (e->hitbox) {
			if (e->hitbox->fixed)
				setlinecolor(RGB(0, 255, 0));
			else
				setlinecolor(RGB(255, 255, 0));

			Box2 box = physics_HitboxAbsolute(e->hitbox);
			rectangle(
				(int)round(box.lefttop.x),
				(int)round(box.lefttop.y),
				(int)round(box.lefttop.x + box.size.x),
				(int)round(box.lefttop.y + box.size.y));
		}
	}
}
}
