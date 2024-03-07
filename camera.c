
#include "camera.h"
#include "types.h"


System_Camera *camera_NewSystem(App *super) {
	System_Camera *sys = zero_malloc(sizeof(System_Camera));
	sys->super         = super;
}
void camera_DeleteSystem(System_Camera *sys);

void camera_Advance(System_Camera *sys, Duration deltaTime);
