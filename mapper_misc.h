#pragma once

#include "types.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _App    App;
typedef struct _Entity Entity;


#define MISC_TEXTBOX_FADEIN_SPEED 5.0 // Fades in/out in 1/5 seconds

// A textbox that shows itself when the player is close
typedef struct {
	char *text;        // Allocated & copied
	Box2  trigger_box; // Relative
	float progress;    // [0.0, 1.0]
	float offset;      // Offset of the top of the text to its origin, usually negative
} misc_Textbox;

// Thinker for textboxes
// Tracks the player & see if they're in the box
void misc_thinker_Textbox(App *app, Entity *e, Duration deltaTime);
// Render callback for textboxes
void misc_render_Textbox(App *app, Entity *e, Vec2 entity_screen_pos, void *user);


// Hazard respawn point
typedef struct {
	Box2 trigger_box;
	Vec2 respawn_pos;
} misc_HazardRespawn;

// Thinker for hazard respawns
// Tracks the player & sets its respawn point
void misc_thinker_HazardRespawn(App *app, Entity *e, Duration deltaTime);


// Misc data an entity in the map might want.
// Used as patches for quick logic like hazard respawn & textbox
typedef struct {
	misc_Textbox       *textbox;
	misc_HazardRespawn *respawn;
	Box2               *hazard; // Harms the player on contact if not null, relative
} Component_Misc;

// Deletes everything nested in misc, and then itself.
void misc_DeleteComponent(Component_Misc *misc);


// Inserts the required components for a textbox.
// Creates misc + render components & sets the thinker.
void misc_InstantiateTextbox(App *app, Entity *e, const char *text, Box2 trigger_box, float offset);


#ifdef __cplusplus
}
#endif
