#include <math.h>
#include <string.h>
#include "core/simulation.h"
#include "core/vec2.h"
#include "core/constants.h"

// Calculates the gravitational force that body b applies to body a.
static Vector2 gravitational_force(const Body *a, const Body *b) {
    // Direction vector from a to b. The final force points along this vector.
    Vector2 r = vec2_vsub(b->position, a->position);

    // EPSILON softens very close interactions so the force does not explode.
    float dist_sq = r.x * r.x + r.y * r.y + EPSILON * EPSILON;
    float inv_dist = 1.0f / sqrtf(dist_sq);
    float inv_dist_cubed = inv_dist * inv_dist * inv_dist;
    // G * m1 * m2 / r^3 times the vector r gives the vector force.
    float force_scale = G * a->mass * b->mass * inv_dist_cubed;

    return vec2_vscale(r, force_scale);
}

// Advances one body's position using its current velocity and acceleration.
static void update_body_position(Body *b, float dt) {
	// Current acceleration comes from F = m * a, rearranged as a = F / m.
	Vector2 b_acc_initial = vec2_vscale(b->force, 1.0 / b->mass);
	
	// r = r + v * dt + (1/2) * a * dt^2
	Vector2 velocity_term_of_position = vec2_vadd(b->position, vec2_vscale(b->velocity, dt)); 
	b->position = vec2_vadd(velocity_term_of_position, vec2_vscale(b_acc_initial, (0.5 * dt * dt)));
}

// Finishes the Velocity Verlet step by updating velocity with old and new acceleration.
static void update_body_velocity(Body *b, Vector2 b_acc_initial, float dt) {
	Vector2 b_acc_new = vec2_vscale(b->force, 1.0 / b->mass);

	// v = v + (1/2) * (a_i + a_f) * dt
	b->velocity = vec2_vadd(b->velocity, vec2_vscale(vec2_vadd(b_acc_initial, b_acc_new), (dt * 0.5)));
}

// Stores body positions for drawing orbit/path trails without saving every frame.
static void body_add_trail_point(Body *body) {
	// Samples a trail point every TRAIL_SAMPLE_INTERVAL frames, so trails aren't too long and cause stack overflow
	body->trail_sample_counter++;

	if (body->trail_sample_counter < TRAIL_SAMPLE_INTERVAL) {
		return;
	}

	body->trail_sample_counter = 0;

	// Circular-buffer index for the next trail sample.
	int index = (body->trail_start + body->trail_count) % TRAIL_MAX;

	body->trail[index] = body->position; 

	if (body->trail_count < TRAIL_MAX) {
		body->trail_count++;
	} else {
		// Once full, advance the start so the oldest point is overwritten.
		body->trail_start = (body->trail_start + 1) % TRAIL_MAX;
	}
}

// Special case for the probe template: these two probes should not pull on each other.
static int bodies_should_ignore_each_other(const Body *a, const Body *b) {
	return ((strcmp(a->name, "Probe A") == 0 && strcmp(b->name, "Probe B") == 0) ||
		(strcmp(a->name, "Probe B") == 0 && strcmp(b->name, "Probe A") == 0));
}

// Recomputes the net gravitational force on every body from all other bodies.
static void calculate_all_forces(Body bodies[], int bodycount) {
    // Clear old forces before accumulating the new forces for this timestep.
    for (int i = 0; i < bodycount; i++) {
	bodies[i].force = (Vector2){0.0f, 0.0f};
    }

    // Visit each pair once, then apply equal and opposite forces to both bodies.
    for (int i = 0; i < bodycount; i++) {
	for (int j = i + 1; j < bodycount; j++) {
	    if (bodies_should_ignore_each_other(&bodies[i], &bodies[j])) {
		continue;
	    }

	    Vector2 force = gravitational_force(&bodies[i], &bodies[j]);

	    bodies[i].force = vec2_vadd(bodies[i].force, force);
	    // Newton's third law: body j gets the same force in the opposite direction.
	    bodies[j].force = vec2_vsub(bodies[j].force, force);
	}
    }
}

// Advances the full simulation by dt seconds using the Velocity Verlet method.
void time_step(Body bodies[], int bodycount, float dt) {
	// Save the starting acceleration because velocity uses the average acceleration.
	Vector2 initial_acc[bodycount];

	// First force pass is based on the positions at the beginning of the timestep.
	calculate_all_forces(bodies, bodycount);

	for (int i = 0; i < bodycount; i++) {
		initial_acc[i] = vec2_vscale(bodies[i].force, 1.0 / bodies[i].mass);
		update_body_position(&bodies[i], dt);
		body_add_trail_point(&bodies[i]);
	}

	// Second force pass uses the new positions so velocity stays more stable.
	calculate_all_forces(bodies, bodycount);

	for (int i = 0; i < bodycount; i++) {
		update_body_velocity(&bodies[i], initial_acc[i], dt);
	}
}
