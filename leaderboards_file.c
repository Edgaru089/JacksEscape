
#include "leaderboards.h"
#include "types.h"
#include <stdio.h>


void lboard_LoadFromFile(System_Leaderboards *sys, const char *filename) {
	FILE *f = fopen(filename, "r");
	if (!f) {
		WARN("failed to open file\"%s\"", filename);
		return;
	}

	int   indice        = -1;
	char *current_level = NULL;


	// Read every line
	char linebuf[512];
	memset(linebuf, 0, sizeof(linebuf));
	while (!feof(f) && fgets(linebuf, sizeof(linebuf), f)) {
		while (linebuf[strlen(linebuf) - 1] == '\n')
			linebuf[strlen(linebuf) - 1] = '\0';

		// Skip empty lines
		if (strlen(linebuf) == 0)
			continue;

		// Look at the first char
		// if '!', begin a new indice
		if (linebuf[0] == '!') {
			indice++;

			if (current_level != NULL)
				free(current_level);
			current_level = copy_malloc(&linebuf[1]);
			continue;
		}

		// Else, find the first space
		// Should have a number in front, and name in the back
		char *space_pos = strchr(linebuf, ' ');
		if (!space_pos)
			// Not found? continue
			continue;
		*space_pos = 0;

		uintptr_t time_millisec = atoll(linebuf);
		lboard_Insert(sys, current_level, time_millisec, space_pos + 1);
		INFO("Inserted @%s %d.%03ds [%s]", current_level, time_millisec / 1000, time_millisec % 1000, space_pos + 1);
	}
	fclose(f);
}
void lboard_SaveToFile(System_Leaderboards *sys, const char *filename) {
	FILE *f = fopen(filename, "w");
	if (!f) {
		WARN("failed to open file\"%s\"", filename);
		return;
	}

	// Dump!
	for (int i = 0; i < vector_Size(sys->records); i++) {
		fprintf(f, "\n!%s\n", *(char **)vector_At(sys->levelnames, i));
		tree_Tree *tree = *(tree_Tree **)vector_At(sys->records, i);
		INFO("Tree %s has %d nodes", *(char **)vector_At(sys->levelnames, i), tree_Count(tree));
		for (tree_Node *node = tree_FirstNode(tree); node != NULL; node = tree_Node_Next(node)) {
			vector_Vector *names         = *(vector_Vector **)node->data;
			uintptr_t      time_millisec = node->key;
			if (names) {
				INFO("Vector %d has %d names", time_millisec, vector_Size(names));
				for (int j = 0; j < vector_Size(names); j++) {
					char *name = *(char **)vector_At(names, j);
					fprintf(f, "%llu %s\n", (unsigned long long)time_millisec, name);
				}
			}
		}
	}

	fclose(f);
}
