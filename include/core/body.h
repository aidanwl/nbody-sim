#ifndef BODY_H
#define BODY_H

#include "core/vec2.h"
#include "constants.h"

// A simulated object with physics state, drawing color, and saved trail samples.
typedef struct {
    char name[32];
    float mass;
    // Position, velocity, and force are in simulation/world coordinates.
    Vector2 position;
    Vector2 velocity;
    Vector2 force;
    Color color;

    // Circular buffer used by the renderer to draw orbit/path history.
    Vector2 trail[TRAIL_MAX];
    int trail_count;
    int trail_start; 
    int trail_sample_counter;

} Body;


#endif
