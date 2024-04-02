#pragma once

#include "types.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _App    App;
typedef struct _Entity Entity;


#define MISC_TEXTBOX_FADEIN_SPEED 6.0 // Fades in/out in 1/6 seconds

// A textbox that shows itself when the player is close
typedef struct {
	char *text;     // Allocated & copied
	float progress; // [0.0, 1.0]
	float offset;   // Offset of the top of the text to its origin, usually negative
} misc_Textbox;

// Thinker for textboxes
// Tracks the player & see if they're in the box
void misc_thinker_Textbox(App *app, Entity *e, Duration deltaTime);
// Render callback for textboxes
void misc_render_Textbox(App *app, Entity *e, Vec2 entity_screen_pos, void *user);


// Thinker for hazard respawns
// Tracks the player & sets its respawn point
void misc_thinker_HazardRespawn(App *app, Entity *e, Duration deltaTime);


// Thinker for hazard areas
// Tracks the player & harms on contact
void misc_thinker_Hazard(App *app, Entity *e, Duration deltaTime);

// Thinker for tolive
// Kills itself after some time
void misc_thinker_ToLive(App *app, Entity *e, Duration deltaTime);

typedef enum {
	misc_Hazard = 1 << 0, // Hazard, harms the player on contact
} misc_TriggerFlags;


// Misc data an entity in the map might want.
// Used as patches for quick logic like hazard respawn & textbox
typedef struct {
	Box2              trigger; // Relative to Position if exists; absolute otherwise
	misc_TriggerFlags trigger_flags;
	misc_Textbox     *textbox;
	Vec2             *respawn_pos; // Set hazard respawn trigger

	TimePoint tolive; // Deletes itself after this time if it is not 0
} Component_Misc;

// Deletes everything nested in misc, and then itself.
void misc_DeleteComponent(Component_Misc *misc);


// Inserts the required components for a textbox.
// Creates misc + render components & sets the thinker.
void misc_InstantiateTextbox(App *app, Entity *e, const char *text, Box2 trigger_box, float offset);

// Inserts the components for a hazard respawn area.
// Creates misc & sets the thinker.
void misc_InstantiateHazardRespawn(App *app, Entity *e, Box2 trigger_box, Vec2 respawn_pos);

// Inserts the components for a hazard harming area.
// Creates misc & sets the thinker.
void misc_InstantiateHazard(App *app, Entity *e, Box2 trigger_box);

// Inserts the components for the entity to kill itself after some time.
// Creates misc & sets the thinker.
//
// You would normally want to set duration to a fixed amount,
// and since to time_Now().
void misc_InstantiateToLive(App *app, Entity *e, Duration duration, TimePoint since);


static inline Box2 misc_TextboxUpright(double width, double height) {
	Box2 b = {
		.lefttop = {
			.x = -width / 2.0,
			.y = -height},
		.size = {.x = width, .y = height}};
	return b;
}

#ifdef __cplusplus
}
#endif
