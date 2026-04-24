#include "core/simulation.h"
#include "core/gravity.h"
#include "core/vec2.h"

void update_body_position(Body *b, float dt) {

	Vector2 b_acc_initial = vec2_vscale(b->force, 1.0 / b->mass);
	
	// r = r + v * dt + (1/2) * a * dt^2
	Vector2 velocity_term_of_position = vec2_vadd(b->position, vec2_vscale(b->velocity, dt)); 
	b->position = vec2_vadd(velocity_term_of_position, vec2_vscale(b_acc_initial, (0.5 * dt * dt)));
}

void update_body_velocity(Body *b, Vector2 b_acc_initial, float dt) {
	Vector2 b_acc_new = vec2_vscale(b->force, 1.0/ b->mass);

	// v = v + (1/2) * (a_i + a_f) * dt
	b->velocity = vec2_vadd(b->velocity, vec2_vscale(vec2_vadd(b_acc_initial, b_acc_new), (dt * 0.5)));

}

void calculate_all_forces(Body bodies[], int bodycount) {
	
	for (int i = 0; i < bodycount; i++) {
        	bodies[i].force = (Vector2){0.0f, 0.0f};
	}

	for (int i = 0; i < bodycount; i++) {
		for (int j = i + 1; j < bodycount; j++) {
			Vector2 force = gravitational_force(&bodies[i], &bodies[j]);

			bodies[i].force = vec2_vadd(bodies[i].force, force);
			bodies[j].force = vec2_vsub(bodies[j].force, force);
        	}
    	}
}

void time_step(Body bodies[], int bodycount, float dt) {

	Vector2 initial_acc[bodycount];

	calculate_all_forces(bodies, bodycount);

	for (int i = 0; i < bodycount; i++) {
        	initial_acc[i] = vec2_vscale(bodies[i].force, 1.0 / bodies[i].mass);
        	update_body_position(&bodies[i], dt);
    	}

	calculate_all_forces(bodies, bodycount);

	for (int i = 0; i < bodycount; i++) {
		update_body_velocity(&bodies[i], initial_acc[i], dt);
	}
}

