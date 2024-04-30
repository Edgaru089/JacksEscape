
#include "leaderboards.h"
#include "types.h"
#include "util/vector.h"
#include "util/tree.h"
#include "util/assert.h"
#include <string.h>


System_Leaderboards *lboard_NewSystem(App *super) {
	System_Leaderboards *sys = zero_malloc(sizeof(System_Leaderboards));
	sys->super               = super;
	sys->levelnames          = vector_Create(sizeof(char *));
	sys->records             = vector_Create(sizeof(tree_Tree *));
	return sys;
}

void lboard_DeleteSystem(System_Leaderboards *sys) {
	// Free level names
	for (int i = 0; i < vector_Size(sys->levelnames); i++) {
		char *str = *(char **)vector_At(sys->levelnames, i);
		free(str);
	}
	vector_Destroy(sys->levelnames);

	// Free records
	for (int i = 0; i < vector_Size(sys->records); i++) {
		tree_Tree *tree = *(tree_Tree **)vector_At(sys->records, i);
		for (tree_Node *node = tree_FirstNode(tree); node != NULL; node = tree_Node_Next(node)) {
			vector_Vector *names = *(vector_Vector **)node->data;
			if (names) {
				for (int j = 0; j < vector_Size(names); j++)
					free(*(void **)vector_At(names, j));
				vector_Destroy(names);
			}
		}
		tree_Destroy(tree);
	}
	vector_Destroy(sys->records);

	free(sys);
}


int lboard_FindLevel(System_Leaderboards *sys, const char *level) {
	for (int i = 0; i < vector_Size(sys->levelnames); i++) {
		char *str = *(char **)vector_At(sys->levelnames, i);
		if (strcmp(str, level) == 0)
			return i;
	}
	return -1;
}

void lboard_Insert(System_Leaderboards *sys, const char *level, uintptr_t time_millisec, const char *name) {
	int indice = lboard_FindLevel(sys, level);
	if (indice == -1) {
		// Append a new one
		char      *level_copy = copy_malloc(level);
		tree_Tree *empty_tree = tree_Create(sizeof(vector_Vector *));
		vector_Push(sys->levelnames, &level_copy);
		vector_Push(sys->records, &empty_tree);
		ASSERT(vector_Size(sys->levelnames) == vector_Size(sys->records));
		indice = vector_Size(sys->levelnames) - 1;
	}

	tree_Tree *tree = *(tree_Tree **)vector_At(sys->records, indice);

	// Insert or find this node
	bool       added;
	tree_Node *node = tree_InsertNode(tree, time_millisec, &added);
	if (added)
		// Brand new node, create a new vector of names
		*((vector_Vector **)node->data) = vector_Create(sizeof(char *));
	// Insert this name
	char *name_copy = copy_malloc(name);
	vector_Push(*(vector_Vector **)node->data, &name_copy);
}
