#include "raylib.h"
#include "render/renderer.h"

void render_body(Vector2 position) {
    DrawCircle(
        (int)position.x,
        (int)position.y,
        5.0f,
        WHITE
    );
}