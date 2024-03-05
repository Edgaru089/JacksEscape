#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#define TEXTHEIGHT 18


// Draws text at the given on-screen position.
//
// unlike outtext(), this function takes consideration
// for newlines.
void render_DrawText(int x, int y, const char *str);


#ifdef __cplusplus
}
#endif
