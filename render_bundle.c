
#include "render_bundle.h"
#include "types.h"
#include "util/vector.h"


/*
render_Primitive *render_NewPrimitive(render_PrimitiveType type) {
	render_Primitive *p = malloc(sizeof(render_Primitive));

	p->type   = type;
	p->points = vector_Create(sizeof(Vec2));
	return p;
}

void render_DeletePrimitive(render_Primitive *p) {
	vector_Destroy(p->points);
	free(p);
}
*/

void render_Push(render_Primitive *p, Vec2 point) {
	vector_Push(p->points, &point);
}


render_Bundle *render_NewBundle(const char *name) {
	render_Bundle *b = malloc(sizeof(render_Bundle));
	b->name          = copy_malloc(name);
	b->prims         = vector_Create(sizeof(render_Primitive));
	return b;
}
void render_DeleteBundle(render_Bundle *b) {
	for (int i = 0; i < vector_Size(b->prims); i++) {
		render_Primitive *p = vector_At(b->prims, i);
		if (p->points)
			vector_Destroy(p->points);
	}
	vector_Destroy(b->prims);
	if (b->name)
		free(b->name);
	free(b);
}

void render_DrawBundleW(App *app, render_Bundle *b, Vec2 offset) {
	for (int i = 0; i < vector_Size(b->prims); i++)
		render_DrawPrimitiveW(app, vector_At(b->prims, i), offset);
}
