#include <math.h>
#include "core/gravity.h"
#include "core/vec2.h"
#include "core/constants.h"

Vector2 gravitational_force(const Body *a, const Body *b) {
	
	Vector2 r = vec2_vsub(b->position, a->position);
	float r_mag = vec2_snorm(r);
	return vec2_vscale(r, (G * a->mass * b->mass) / pow((pow(r_mag, 2) + pow(EPSILON, 2)), 1.5));
}


