#include "physics/simulation.h"
#include "physics/gravity.h"
#include "math/vec2.h"

void update_body(Body *b, double dt) {
    Vector2 acc = vec2_vscale(b->force, 1.0 / b->mass);

    b->velocity = vec2_vadd(b->velocity, vec2_vscale(acc, dt));
    b->position = vec2_vadd(b->position, vec2_vscale(b->velocity, dt));
}

void update_all_bodies(Body bodies[], int bodycount, double dt) {
    for (int i = 0; i < bodycount; i++) {
        update_body(&bodies[i], dt);
    }
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

void time_step(Body bodies[], int bodycount, double dt) {
    calculate_all_forces(bodies, bodycount);
    update_all_bodies(bodies, bodycount, dt);
}