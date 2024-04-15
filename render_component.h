#pragma once

#include "render_bundle.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _App    App;
typedef struct _Entity Entity;

typedef void (*render_CustomFunc)(App *app, Entity *e, Vec2 entity_screen_pos, void *user);

// Rendering component.
// This is mostly for components requiring static renders.
typedef struct {
	Entity *super;

	render_Bundle    *bundle;      // A render bundle, usually found by render_FindBundle()
	render_CustomFunc custom;      // Custom rendering function
	void             *custom_data; // User data for the callback

	Box2     fillbox;   // Fill box
	uint32_t fillcolor; // Fill color
} Component_Render;


// Creates a new component with a static render bundle
Component_Render *render_NewComponent(App *app, const char *bundle_name);
// Creates a new component with a callback for rendering
Component_Render *render_NewComponentFunc(App *app, render_CustomFunc func, void *data);
void              render_DeleteComponent(Component_Render *r);


#ifdef __cplusplus
}
#endif
