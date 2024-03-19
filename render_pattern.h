#pragma once

// for BYTE
#include <minwindef.h>

// Returns a dissolve pattern for the given progress in [0,1]
BYTE *render_DissolvePattern(double progress);

// The underlying data. 520 bytes
extern char render_DissolvePatternData[520];
