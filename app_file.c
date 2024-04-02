
#include <stdio.h>
#include "app.h"
#include "types.h"


void app_QueueLoadLevel(App *app, const char *level_name) {
	if (app->switch_level != NULL) {
		WARN("previous switch_level \"%s\" not processed; purged", app->switch_level);
		free(app->switch_level);
	}
	app->switch_level = copy_malloc(level_name);
}

void _app_SwitchLevel(App *app) {
}
