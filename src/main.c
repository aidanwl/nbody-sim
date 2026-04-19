#include "raylib.h"

#include "core/body.h"
#include "physics/simulation.h"
#include "render/renderer.h"
#include "config/constants.h"
#include <math.h>

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "NBody Sim");
    SetTargetFPS(60);

    Body bodies[3];

    // Central body
    bodies[0].mass = 1000.0;
    bodies[0].position = (Vector2){400.0f, 300.0f};
    bodies[0].velocity = (Vector2){0.0f, 0.0f};
    bodies[0].force = (Vector2){0.0f, 0.0f};

    // Orbiting body
    bodies[1].mass = 10.0;
    bodies[1].position = (Vector2){414.0f, 300.0f};
    bodies[1].velocity = (Vector2){0.0f, sqrt(G * bodies[0].mass / 10.0)};
    bodies[1].force = (Vector2){0.0f, 0.0f};

	// Orbiting body 2
    bodies[2].mass = 1.0;
    bodies[2].position = (Vector2){490.0f, 300.0f};
    bodies[2].velocity = (Vector2){0.0f, sqrt(G * bodies[1].mass / 10.0)};
    bodies[2].force = (Vector2){0.0f, 0.0f};

    double dt = 1.0 / 40.0;

    while (!WindowShouldClose()) {
        time_step(bodies, 3, dt);

        BeginDrawing();
        ClearBackground(BLACK);	

        render_body(bodies[0].position);
        render_body(bodies[1].position);
		render_body(bodies[2].position);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}