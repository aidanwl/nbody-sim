#ifndef GRAVITY_H
#define GRAVITY_H

#include "core/body.h"
#include "math/vec2.h"

float gravitational_force_magnitude(const Body *a, const Body *b);
Vector2 gravitational_force(const Body *a, const Body *b);

#endif