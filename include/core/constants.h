#ifndef CONSTANTS_H
#define CONSTANTS_H

// Tuned gravitational constant for the visual simulation scale.
#define G 1.0f

// Softening term that prevents divide-by-zero or extreme force spikes.
#define EPSILON 1e-9f

// Maximum stored trail points per body and how often a point is recorded.
#define TRAIL_MAX 12000
#define TRAIL_SAMPLE_INTERVAL 16


#endif
