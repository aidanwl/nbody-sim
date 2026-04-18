#include <stdio.h>
#include "vecmath.h"
#include "update_body.h"
#include <math.h>

int main(){
    int n = 2;
    Body bodies[2];

    bodies[0].mass = 1000.0;
    bodies[0].position = (Vector2){0.0, 0.0};
    bodies[0].velocity = (Vector2){0.0, 0.0};

    bodies[1].mass = 1.0;
    bodies[1].position = (Vector2){10.0, 0.0};

    double r = 10.0;
    double v = sqrt(G * bodies[0].mass / r);

    bodies[1].velocity = (Vector2){0.0, v};

    double dt = 0.001;   
    int steps = 10000;

    for(int i = 0; i < steps; i++){
        time_step(bodies, n, dt);

        printf("%f %f\n", bodies[1].position.x, bodies[1].position.y);
    }

    return 0;
}
