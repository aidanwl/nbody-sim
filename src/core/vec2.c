#include <math.h>
#include "core/vec2.h"
#include "core/constants.h"

// ---------- Vector Return ------------------------

Vector2 vec2_vadd(Vector2 a, Vector2 b) {
	return (Vector2){a.x + b.x, a.y + b.y};
}

Vector2 vec2_vsub(Vector2 a, Vector2 b) {
	return (Vector2){a.x - b.x, a.y - b.y};
}

Vector2 vec2_vscale(Vector2 v, float s) {
	return (Vector2){v.x * s, v.y * s};
}

// ---------- Scalar Return ------------------------

float vec2_snorm(Vector2 v) {
	return hypot(v.x, v.y);
}
