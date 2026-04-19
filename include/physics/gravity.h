#ifndef GRAVITY_H
#define GRAVITY_H

#include "body.h"
#include "vec2.h"

double gravitational_force_magnitude(const Body *a, const Body *b);
Vector2 gravitational_force(const Body *a, const Body *b);

#endif