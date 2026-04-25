#ifndef VEC2_H
#define VEC2_H

#include "raylib.h"

// ---------- Vector Return ------------------------

Vector2 vec2_vadd(Vector2 a, Vector2 b);
Vector2 vec2_vsub(Vector2 a, Vector2 b);
Vector2 vec2_vscale(Vector2 v, float s);
Vector2 vec2_vnorm(Vector2 v);

// ---------- Scalar Return ------------------------

float vec2_snorm(Vector2 v);
float vec2_sdistance(Vector2 a, Vector2 b);

#endif
