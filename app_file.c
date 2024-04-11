
#include <stdio.h>
#include "app.h"
#include "particle.h"
#include "types.h"


void app_QueueLoadLevel(App *app, const char *level_name) {
	if (app->switch_level != NULL) {
		WARN("previous switch_level \"%s\" not processed; purged", app->switch_level);
		free(app->switch_level);
	}
	app->switch_level = copy_malloc(level_name);
}


#define CMD1(str)    if (strcmp(cmd, str) == 0)
#define CMD(str)     else if (strcmd(cmd, str) == 0)
#define TOKEN        (strtok(NULL, " "))
#define TOKEN_INT    (atoi(TOKEN))
#define TOKEN_DOUBLE (strtod(TOKEN))

// Subsequent tokens can be read by strtok(NULL, " ")
static void _app_LevelCommand(char *cmd) {
}


// Defined in render_bundle_file.c
extern char linebuf[512];

void _app_SwitchLevel(App *app) {
	if (app->switch_level == NULL) {
		WARN("called when switch_level is NULL", 0);
		return;
	}

	FILE *f = fopen(app->switch_level, "r");
	if (!f) {
		WARN("failed to open file\"%s\"", app->switch_level);
		return;
	}

	// Clear the current level
	entity_Clear(app->entity);
	particle_Clear(app->particle);

	// Read every line
	while (!feof(f) && fgets(linebuf, sizeof(linebuf), f)) {
		while (linebuf[strlen(linebuf) - 1] == '\n')
			linebuf[strlen(linebuf) - 1] = '\0';

		char *cmd = strtok(linebuf, " ");
		if (cmd == NULL)
			continue;
		_app_LevelCommand(cmd);
	}


	free(app->switch_level);
	app->switch_level = NULL;
}
