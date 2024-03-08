#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


static inline void *zero_malloc(size_t size) {
	void *d = malloc(size);
	memset(d, 0, size);
	return d;
}


// A 2d vector of double.
typedef struct {
	double x, y;
} Vec2;


static inline Vec2 vec2(double x, double y) {
	Vec2 v = {.x = x, .y = y};
	return v;
}

Vec2 vec2_Add(Vec2 x, Vec2 y);
Vec2 vec2_Minus(Vec2 pos, Vec2 neg);
Vec2 vec2_Scale(Vec2 v, double scale);


// A 2d box of double.
typedef struct {
	Vec2 lefttop;
	Vec2 size;
} Box2;


// Intersection test.
bool box2_Intersects(const Box2 x, const Box2 y, Box2 *out_intersection);

Vec2 box2_Center(Box2 box);
Box2 box2_FromCenter(Vec2 center, Vec2 size);

Box2 box2_Offset(Box2 box, Vec2 offset);
Box2 box2_OffsetX(Box2 box, double offsetX);
Box2 box2_OffsetY(Box2 box, double offsetY);


// Time duration.
typedef struct {
	int64_t microseconds;
} Duration;

static inline double duration_Seconds(const Duration t) { return ((double)t.microseconds) / 1000000.0; }
static inline double duration_Milliseconds(const Duration t) { return ((double)t.microseconds) / 1000.0; }
// This function has a precision of at most 1ms under Windows. Sad
void duration_Sleep(const Duration t);

// A Time point, counted since a fixed point in the past.
typedef struct {
	int64_t microseconds;
} TimePoint;

TimePoint time_Now();
Duration  time_Since(TimePoint prev);
Duration  time_Difference(TimePoint now, TimePoint prev);
Duration  time_Reset(TimePoint *prev);

// 1e-6
extern const double EPS;


#ifdef __cplusplus
}
#endif
