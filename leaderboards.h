#pragma once

#include "util/vector.h"
#include "util/tree.h"
#include "types.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _App App;


// Leaderboards keep track of level-gametime-username records in leaderboards.txt.
typedef struct {
	App *super;

	vector_Vector *levelnames; // vector of allocated char* strings
	vector_Vector *records;    // vector of tree_Tree* of gametime(milliseconds)->vector of allocated char* strings
} System_Leaderboards;


System_Leaderboards *lboard_NewSystem(App *super);
void                 lboard_DeleteSystem(System_Leaderboards *sys);

// Find the indice at which this level is located
// Returns -1 if not found
int lboard_FindLevel(System_Leaderboards *sys, const char *level);
// Insert a record.
void lboard_Insert(System_Leaderboards *sys, const char *level, uintptr_t time_millisec, const char *name);


// This should only be called on an empty (newly created) leaderboard.
void lboard_LoadFromFile(System_Leaderboards *sys, const char *filename);
void lboard_SaveToFile(System_Leaderboards *sys, const char *filename);


#ifdef __cplusplus
}
#endif
