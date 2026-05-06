#include "raylib.h"
#include "core/renderer.h"

// Draws a body as a simple filled circle after world-to-screen conversion.
void render_body(Vector2 position, float radius, Color color) {
    DrawCircle(
        (int)position.x,
        (int)position.y,
        radius,
        color
    );
}
