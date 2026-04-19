#include <math.h>
#include "vec2.h"
#include "constants.h"

// ---------- Vector Return ------------------------

Vector2 vec2_vadd(Vector2 a, Vector2 b) {
	return (Vector2){a.x + b.x, a.y + b.y};
}

Vector2 vec2_vsub(Vector2 a, Vector2 b) {
	return (Vector2){a.x - b.x, a.y - b.y};
}

Vector2 vec2_vscale(Vector2 v, double s) {
	return (Vector2){v.x * s, v.y * s};
}

Vector2 vec2_vnorm(Vector2 v) {
	double len = vec2_snorm(v);

	if (len < EPSILON) {
		return (Vector2){0.0, 0.0};
	}
	
	return vec2_vscale(v, 1.0 / len);
}

// ---------- Scalar Return ------------------------

double vec2_snorm(Vector2 v) {
	return hypot(v.x, v.y);
}

double vec2_sdistance(Vector2 a, Vector2 b) {
	return vec2_snorm(vec_sub(a, b));
}