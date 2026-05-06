#ifndef SIMULATION_H
#define SIMULATION_H

#include "core/body.h"

// Advances all bodies by one simulation step.
void time_step(Body bodies[], int bodycount, float dt);

#endif
