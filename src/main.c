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
		FILE *f = fopen("output.txt", "w");

		for(int i = 0; i < steps; i++){
		time_step(bodies, n, dt);
	
			for(int j = 0; j < n; j++){
			fprintf(f, "%f %f ", bodies[j].position.x, bodies[j].position.y);
			}
    		fprintf(f, "\n");
		}

	fclose(f);
    	}

    return 0;
}
