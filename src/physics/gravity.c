#include <math.h>
#include "gravity.h"
#include "vec2.h"

double gravitational_force_magnitude(const Body *a, const Body *b) {
    Vector2 r = vec2_vsub(b->position, a->position);
    double dist_sq = r.x * r.x + r.y * r.y + EPSILON * EPSILON;

    return (G * a->mass * b->mass) / dist_sq;
}

Vector2 gravitational_force(const Body *a, const Body *b) {
	return vec_scale(vec_norm(vec_sub(b->position, a->position)), force_mag(a, b)); 
}
