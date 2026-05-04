#include <math.h>
#include "core/simulation.h"
#include "core/vec2.h"
#include "core/constants.h"

static Vector2 gravitational_force(const Body *a, const Body *b) {
    Vector2 r = vec2_vsub(b->position, a->position);

    float dist_sq = r.x * r.x + r.y * r.y + EPSILON * EPSILON;
    float inv_dist = 1.0f / sqrtf(dist_sq);
    float inv_dist_cubed = inv_dist * inv_dist * inv_dist;
    float force_scale = G * a->mass * b->mass * inv_dist_cubed;

    return vec2_vscale(r, force_scale);
}


static void update_body_position(Body *b, float dt) {

	Vector2 b_acc_initial = vec2_vscale(b->force, 1.0 / b->mass);
	
	// r = r + v * dt + (1/2) * a * dt^2
	Vector2 velocity_term_of_position = vec2_vadd(b->position, vec2_vscale(b->velocity, dt)); 
	b->position = vec2_vadd(velocity_term_of_position, vec2_vscale(b_acc_initial, (0.5 * dt * dt)));
}

static void update_body_velocity(Body *b, Vector2 b_acc_initial, float dt) {
	Vector2 b_acc_new = vec2_vscale(b->force, 1.0/ b->mass);

	// v = v + (1/2) * (a_i + a_f) * dt
	b->velocity = vec2_vadd(b->velocity, vec2_vscale(vec2_vadd(b_acc_initial, b_acc_new), (dt * 0.5)));

}

static void body_add_trail_point(Body *body) {
	body->trail_sample_counter++;

	if (body->trail_sample_counter < TRAIL_SAMPLE_INTERVAL) {
		return;
	}

	body->trail_sample_counter = 0;

	int index = (body->trail_start + body->trail_count) % TRAIL_MAX;

	body->trail[index] = body->position; 

	if (body->trail_count < TRAIL_MAX) {
		body->trail_count++;
	} else {
		body->trail_start = (body->trail_start + 1) % TRAIL_MAX;
	}
}



static void calculate_all_forces(Body bodies[], int bodycount) {
	
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
		body_add_trail_point(&bodies[i]);
    	}

	calculate_all_forces(bodies, bodycount);

	for (int i = 0; i < bodycount; i++) {
		update_body_velocity(&bodies[i], initial_acc[i], dt);
	}
}
