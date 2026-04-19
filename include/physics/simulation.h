#ifndef SIMULATION_H
#define SIMULATION_H
#include "core/body.h"

void update_body(Body *b, double dt);
void update_all_bodies(Body bodies[], int count, double dt);
void calculate_all_forces(Body bodies[], int count);
void time_step(Body bodies[], int count, double dt);

#endif
