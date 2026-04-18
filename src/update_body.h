#ifndef UPDATE_BODY_H
#define UPDATE_BODY_H
#include "vecmath.h"
#include <math.h>

void update_body(Body *b, double dt);
void update_all_bodies(Body bodies[], int bodycount, double dt);
void calculate_all_forces(Body bodies[], int bodycount);
void time_step(Body bodies[], int bodycount, double dt);

#endif
