#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif


#define SCREEN_WIDTH  1600
#define SCREEN_HEIGHT 900

#define WARN(fmt, ...) fprintf(stderr, "[WARN][%s] " fmt "\n", __func__, ##__VA_ARGS__)
#define INFO(fmt, ...) fprintf(stderr, "[%s] " fmt "\n", __func__, ##__VA_ARGS__)


static inline void *zero_malloc(size_t size) {
	void *d = malloc(size);
	memset(d, 0, size);
	return d;
}

static inline char *copy_malloc(const char *src) {
	size_t len = strlen(src);
	char  *p   = (char *)malloc(len + 1);
	memcpy(p, src, len + 1);
	return p;
}

static inline void *copy_malloc_size(void *src, size_t size) {
	void *p = malloc(size);
	memcpy(p, src, size);
	return p;
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

Vec2 vec2_Random(double minX, double maxX, double minY, double maxY);


// A 2d box of double.
typedef struct {
	Vec2 lefttop;
	Vec2 size;
} Box2;

static inline Box2 box2(double x, double y, double width, double height) {
	Box2 b;
	b.lefttop.x = x;
	b.lefttop.y = y;
	b.size.x    = width;
	b.size.y    = height;
	return b;
}


// Intersection test.
bool box2_Intersects(const Box2 x, const Box2 y, Box2 *out_intersection);
// Contain test.
bool box2_Contains(const Box2 box, const Vec2 point);

Vec2 box2_Center(Box2 box);
Box2 box2_FromCenter(Vec2 center, Vec2 size);

Box2 box2_Offset(Box2 box, Vec2 offset);
Box2 box2_OffsetX(Box2 box, double offsetX);
Box2 box2_OffsetY(Box2 box, double offsetY);

// Returns true if the area of the box is not 0.
bool box2_NotZero(Box2 box);


// Time duration.
typedef struct {
	int64_t microseconds;
} Duration;

static inline double   duration_Seconds(const Duration t) { return ((double)t.microseconds) / 1000000.0; }
static inline double   duration_Milliseconds(const Duration t) { return ((double)t.microseconds) / 1000.0; }
static inline Duration duration_FromSeconds(double seconds) {
	Duration d = {.microseconds = (int64_t)round(seconds * 1000.0 * 1000.0)};
	return d;
}
// This function has a precision of at most 1ms under Windows. Sad
void duration_Sleep(const Duration t);

// A Time point, counted since a fixed point in the past.
typedef struct {
	int64_t microseconds;
} TimePoint;

TimePoint time_Now();
TimePoint time_After(TimePoint now, Duration after);
Duration  time_Since(TimePoint prev);
Duration  time_Difference(TimePoint now, TimePoint prev);
Duration  time_Reset(TimePoint *prev);

// 1e-6
extern const double EPS;


#ifdef __cplusplus
}
#endif
