#pragma once

// for BYTE
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

// Returns a dissolve pattern for the given progress in [0,1]
// From empty to full
BYTE *render_DissolvePatternIn(double progress);
// From full to empty
BYTE *render_DissolvePatternOut(double progress);


// The underlying data. 520 bytes
extern char render_DissolvePatternData[520];


#ifdef __cplusplus
}
#endif
