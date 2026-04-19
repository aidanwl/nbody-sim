#ifndef BODY_H
#define BODY_H

#include "math/vec2.h"

typedef struct {
    float mass;
    Vector2 position;
    Vector2 velocity;
    Vector2 force;
} Body;

#endif
