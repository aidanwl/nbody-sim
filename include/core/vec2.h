#ifndef VEC2_H
#define VEC2_H

#include "raylib.h"

// Small vector helpers used by physics, camera movement, and rendering.
Vector2 vec2_vadd(Vector2 a, Vector2 b);
Vector2 vec2_vsub(Vector2 a, Vector2 b);
Vector2 vec2_vscale(Vector2 v, float s);

#endif
