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


Vec2 vec2_Add(Vec2 x, Vec2 y);
Vec2 vec2_Scale(Vec2 v, double scale);


// A 2d box of double.
typedef struct {
	Vec2 lefttop;
	Vec2 size;
} Box2;


// Intersection test.
bool box2_Intersects(const Box2 x, const Box2 y, Box2 *out_intersection);


#ifdef __cplusplus
}
#endif