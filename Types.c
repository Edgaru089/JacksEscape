
#include "Types.h"
#include <windows.h>

static inline double dmin(double x, double y) {
	return x < y ? x : y;
}
static inline double dmax(double x, double y) {
	return x > y ? x : y;
}

const double EPS = 1e-6;


Vec2 vec2_Add(Vec2 x, Vec2 y) {
	Vec2 result = {
		.x = x.x + y.x,
		.y = x.y + y.y};
	return result;
}

Vec2 vec2_Scale(Vec2 v, double scale) {
	Vec2 result = {
		.x = v.x * scale,
		.y = v.y * scale};
	return result;
}

bool box2_Intersects(const Box2 x, const Box2 y, Box2 *out_intersection) {
	// Compute the min and max of the first rectangle on both axes
	const double r1MinX = dmin(x.lefttop.x, x.lefttop.x + x.size.x);
	const double r1MaxX = dmax(x.lefttop.x, x.lefttop.x + x.size.x);
	const double r1MinY = dmin(x.lefttop.y, x.lefttop.x + x.size.x);
	const double r1MaxY = dmax(x.lefttop.y, x.lefttop.x + x.size.x);

	// Compute the min and max of the second rectangle on both axes
	const double r2MinX = dmin(y.lefttop.x, y.lefttop.x + y.size.x);
	const double r2MaxX = dmax(y.lefttop.x, y.lefttop.x + y.size.x);
	const double r2MinY = dmin(y.lefttop.y, y.lefttop.y + y.size.y);
	const double r2MaxY = dmax(y.lefttop.y, y.lefttop.y + y.size.y);

	// Compute the intersection boundaries
	const double interLeft   = dmax(r1MinX, r2MinX);
	const double interTop    = dmax(r1MinY, r2MinY);
	const double interRight  = dmin(r1MaxX, r2MaxX);
	const double interBottom = dmin(r1MaxY, r2MaxY);

	// If the intersection is valid (positive non zero area), then there is an intersection
	if ((interLeft < interRight) && (interTop < interBottom)) {
		if (out_intersection) {
			out_intersection->lefttop.x = interLeft;
			out_intersection->lefttop.y = interTop;
			out_intersection->size.x    = interRight - interLeft;
			out_intersection->size.y    = interBottom - interTop;
		}
		return true;
	} else {
		return false;
	}
}

Box2 box2_Offset(Box2 box, Vec2 offset) {
	box.lefttop = vec2_Add(box.lefttop, offset);
	return box;
}
Box2 box2_OffsetX(Box2 box, double offsetX) {
	box.lefttop.x += offsetX;
	return box;
}
Box2 box2_OffsetY(Box2 box, double offsetY) {
	box.lefttop.y += offsetY;
	return box;
}


static double freqInverse = 0.0;

TimePoint time_Now() {
	// Reference: https://github.com/SFML/SFML/blob/2.6.x/src/SFML/System/Win32/ClockImpl.cpp

	if (freqInverse == 0.0) {
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		freqInverse = 1000000.0 / ((double)freq.QuadPart);
	}

	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);

	TimePoint t = {.microseconds = (int64_t)(((double)time.QuadPart) * freqInverse)};
	return t;
}

Duration time_Since(TimePoint prev) {
	return time_Difference(time_Now(), prev);
}
Duration time_Difference(TimePoint now, TimePoint prev) {
	Duration d = {.microseconds = now.microseconds - prev.microseconds};
	return d;
}
