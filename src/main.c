#include "raylib.h"
#include "app/app.h"

int main() {
    const int screen_width = 1100;
    const int screen_height = 750;

    InitWindow(screen_width, screen_height, "NBody Sim");
    SetTargetFPS(60);

    static App app;
    app_init(&app, screen_width, screen_height);

    while (!WindowShouldClose()) {
        app_update(&app);

        BeginDrawing();
        ClearBackground(BLACK);
        app_draw(&app);
        EndDrawing();
    }
    
    app_deinit(&app);
    CloseWindow();
    return 0;
}
