
#include "render_component.h"
#include "app.h"
#include "render_bundle.h"
#include "types.h"
#include "util/vector.h"


Component_Render *render_NewComponent(Entity *super, const char *bundle_name) {
	render_Bundle *b = render_FindBundle(bundle_name);
	if (b == NULL) // Not found
		return NULL;

	Component_Render *r = zero_malloc(sizeof(Component_Render));
	r->super            = super;
	r->bundle           = b;
	return r;
}

Component_Render *render_NewComponentFunc(Entity *super, render_CustomFunc func, void *data) {
	Component_Render *r = zero_malloc(sizeof(Component_Render));
	r->super            = super;
	r->custom           = func;
	r->custom_data      = data;
	return r;
}

void render_DeleteComponent(Component_Render *r) {
	if (r->fillpoly)
		vector_Destroy(r->fillpoly);
	free(r);
}
