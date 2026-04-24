#ifndef SIMULATION_H
#define SIMULATION_H

#include "core/body.h"

void update_body(Body *b, float dt);
void update_all_bodies(Body bodies[], int bodycount, float dt);
void calculate_all_forces(Body bodies[], int bodycount);
void time_step(Body bodies[], int bodycount, float dt);

#endif