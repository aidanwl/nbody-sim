#include "vecmath.h"

void update_body(Body *b, double dt){

	Vector2 acc = vec_scale(b->force, 1.0 / b->mass);
	b->velocity = vec_add(b->velocity, vec_scale(acc, dt));
	b->position = vec_add(b->position, vec_scale(b->velocity, dt));

}

void update_all_bodies(Body bodies[], int bodycount, double dt){

	for(int i = 0; i < bodycount; i++){
		update_body(&bodies[i], dt);
	}
}

void calculate_all_forces(Body bodies[], int bodycount){

	for(int i = 0; i < bodycount; i++){
		bodies[i].force = (Vector2){0.0, 0.0};
	}

	for(int i = 0; i < bodycount; i++){
		for(int j = 0; j < bodycount; j++){
			Vector2 force = vec_force(&bodies[i], &bodies[j]);
			
			bodies[i].force = vec_add(bodies[i].force, force);
			bodies[j].force = vec_sub(bodies[j].force, force);
			}
		}
	}

void time_step(Body bodies[], int bodycount, double dt){

	calculate_all_forces(bodies, bodycount); 
	update_all_bodies(bodies, bodycount, dt);

}

