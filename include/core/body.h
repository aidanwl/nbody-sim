#ifndef BODY_H
#define BODY_H

#include "core/vec2.h"
#include "constants.h"

typedef struct {
    float mass;
    Vector2 position;
    Vector2 velocity;
    Vector2 force;

    Vector2 trail[TRAIL_MAX];
    int trail_count;
    int trail_start; 

} Body;


#endif
