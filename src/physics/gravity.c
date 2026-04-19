#include <math.h>
#include "physics/gravity.h"
#include "math/vec2.h"
#include "config/constants.h"

float gravitational_force_magnitude(const Body *a, const Body *b) {
    Vector2 r = vec2_vsub(b->position, a->position);
    float dist_sq = r.x * r.x + r.y * r.y + EPSILON * EPSILON;

    return (float)((G * a->mass * b->mass) / dist_sq);
}

Vector2 gravitational_force(const Body *a, const Body *b) {
    Vector2 direction = vec2_vnorm(vec2_vsub(b->position, a->position));
    float magnitude = gravitational_force_magnitude(a, b);

    return vec2_vscale(direction, magnitude);
}