#include "raylib.h"
#include "app/app.h"

// Creates the raylib window and runs the app frame loop.
int main() {
    const int screen_width = 1100;
    const int screen_height = 750;

    // raylib owns the OS window; the App struct owns game/simulation state.
    InitWindow(screen_width, screen_height, "NBody Sim");
    SetTargetFPS(60);

    static App app;
    app_init(&app, screen_width, screen_height);

    while (!WindowShouldClose()) {
        // Update state first, then draw the current frame.
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
