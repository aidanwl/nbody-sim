#include "physics/simulation.h"
#include "physics/gravity.h"
#include "math/vec2.h"

void update_body(Body *b, double dt) {
	Vector2 acc = vec_scale(b->force, 1.0 / b->mass);
	b->velocity = vec_add(b->velocity, vec_scale(acc, dt));
	b->position = vec_add(b->position, vec_scale(b->velocity, dt));
}

void update_all_bodies(Body bodies[], int count, double dt) {
	for(int i = 0; i < count; i++) {
		update_body(&bodies[i], dt);
	}
}

void calculate_all_forces(Body bodies[], int count) {
	for(int i = 0; i < count; i++) {
		bodies[i].force = (Vector2){0.0, 0.0};
	}

	for(int i = 0; i < count; i++) {
		for(int j = 0; j < count; j++) {
			Vector2 force = vec_force(&bodies[i], &bodies[j]);
			
			bodies[i].force = vec_add(bodies[i].force, force);
			bodies[j].force = vec_sub(bodies[j].force, force);
		}
	}
}

void time_step(Body bodies[], int count, double dt) {
	calculate_all_forces(bodies, count); 
	update_all_bodies(bodies, count, dt);
}

