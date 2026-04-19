#ifndef BODY_H
#define BODY_H

#include "vec2.h"

typedef struct {
    double mass;
    Vector2 position;
    Vector2 velocity;
} Body;

#endif
