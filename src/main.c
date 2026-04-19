#include "raylib.h"

#include "math/vec2.h"
#include "core/body.h"
#include "physics/simulation.h"
#include "render/renderer.h"

int main(void) {

    // --- Window setup ---
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Physics Sim");
    SetTargetFPS(60);

    // --- Simulation setup ---
    Body bodies[1];

    bodies[0].mass = 10.0;
    bodies[0].position = (Vector2){400.0, 300.0};
    bodies[0].velocity = (Vector2){50.0, 0.0};

    float dt = 1.0 / 60.0;

    while (!WindowShouldClose()) {

        time_step(bodies, 1, dt);

        BeginDrawing();
        ClearBackground(BLACK);

        render_body(bodies[0].position);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}