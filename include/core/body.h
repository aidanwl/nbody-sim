#ifndef BODY_H
#define BODY_H

#include "core/vec2.h"
#include "constants.h"

typedef struct {
    char name[32];
    float mass;
    Vector2 position;
    Vector2 velocity;
    Vector2 force;
    Color color;

    Vector2 trail[TRAIL_MAX];
    int trail_count;
    int trail_start; 
    int trail_sample_counter;

} Body;


#endif
