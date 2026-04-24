#include "raylib.h"
#include "app/app.h"

int main() {
    const int screen_width = 800;
    const int screen_height = 600;

    InitWindow(screen_width, screen_height, "NBody Sim");
    SetTargetFPS(60);

    App app;
    app_init(&app, screen_width, screen_height);

    while (!WindowShouldClose()) {
        app_update(&app);

        BeginDrawing();
        ClearBackground(BLACK);
        app_draw(&app);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
