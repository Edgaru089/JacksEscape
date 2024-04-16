#pragma once

#include "render_util.h"
#include "types.h"
#include "util/vector.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	render_Lines,     // Individial lines
	render_LineStrip, // Connected lines
	render_Polygon    // Filled polygon
} render_PrimitiveType;


// render_Primitive describes one render operation.
typedef struct {
	FillMode mode; // fill mode

	render_PrimitiveType type;
	vector_Vector       *points; // Vector of Vec2
} render_Primitive;

// render_Primitive *render_NewPrimitive(render_PrimitiveType type);
// void              render_DeletePrimitive(render_Primitive *p);

void render_Push(render_Primitive *p, Vec2 point);


typedef struct _App App;

// Draws a primitive with the given offset in screen space.
void render_DrawPrimitiveW(App *app, render_Primitive *p, Vec2 offset);


// render_Bundle is a list of Primitives
typedef struct {
	char          *name;  // Name, malloc & copied
	vector_Vector *prims; // Vector of render_Primitive
} render_Bundle;

// Creates a new bundle. Name is copied into a malloc'ed string.
render_Bundle *render_NewBundle(const char *name);
void           render_DeleteBundle(render_Bundle *b);

// Draws the bundle with the given offset in screen space.
void render_DrawBundleW(App *app, render_Bundle *b, Vec2 offset);


// Global bundles identified by name
extern vector_Vector *render_Bundles;

// Appends the global bundle list with this file.
void render_LoadBundle(const char *filename);
// Finds the first bundle with this name, or NULL otherwise.
render_Bundle *render_FindBundle(const char *name);


#ifdef __cplusplus
}
#endif
