#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


// A 2d vector of double.
typedef struct {
	double x, y;
} Vec2;


inline Vec2 vec2(double x, double y) {
	Vec2 v = {.x = x, .y = y};
	return v;
}

Vec2 vec2_Add(Vec2 x, Vec2 y);
Vec2 vec2_Scale(Vec2 v, double scale);


// A 2d box of double.
typedef struct {
	Vec2 lefttop;
	Vec2 size;
} Box2;


// Intersection test.
bool box2_Intersects(const Box2 x, const Box2 y, Box2 *out_intersection);

Box2 box2_Offset(Box2 box, Vec2 offset);
Box2 box2_OffsetX(Box2 box, double offsetX);
Box2 box2_OffsetY(Box2 box, double offsetY);


// Time duration.
typedef struct {
	uint64_t microseconds;
} Duration;

static inline double duration_Seconds(const Duration t) { return ((double)t.microseconds) / 1000.0 / 1000.0; }
static inline double duration_Milliseconds(const Duration t) { return ((double)t.microseconds) / 1000.0; }

// Returns a relative duration since
// a static time point in the past.
//
// Its absolute value has no meaning.
Duration duration_Now();


#ifdef __cplusplus
}
#endif
