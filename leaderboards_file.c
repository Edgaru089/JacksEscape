
#include "leaderboards.h"
#include <stdio.h>


void lboard_LoadFromFile(System_Leaderboards *sys, const char *filename) {
	FILE *f = fopen(filename, "r");
	if (!f) {
		WARN("failed to open file\"%s\"", filename);
		return;
	}


	// Read every line
	char linebuf[512];
	memset(linebuf, 0, sizeof(linebuf));
	while (!feof(f) && fgets(linebuf, sizeof(linebuf), f)) {
		while (linebuf[strlen(linebuf) - 1] == '\n')
			linebuf[strlen(linebuf) - 1] = '\0';

		// Skip empty lines
		if (strlen(linebuf) == 0)
			continue;
	}
}
void lboard_SaveToFile(System_Leaderboards *sys, const char *filename) {
}
