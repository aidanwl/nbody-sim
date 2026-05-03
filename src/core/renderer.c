#include "raylib.h"
#include "core/renderer.h"
#include "core/constants.h"
#include "core/vec2.h"

void render_body(Vector2 position, float radius, Color color) {
    DrawCircle(
        (int)position.x,
        (int)position.y,
        radius,
        color
    );
}
