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

void render_trail(const Body *body, float zoom, Vector2 camera_offset) {
    for (int i = 1; i < body->trail_count; i++) {
        int a = (body->trail_start + i - 1) % TRAIL_MAX;
        int b = (body->trail_start + i) % TRAIL_MAX;

        Vector2 p1 = vec2_vadd(vec2_vscale(body->trail[a], zoom), camera_offset);
        Vector2 p2 = vec2_vadd(vec2_vscale(body->trail[b], zoom), camera_offset);

        DrawLineEx(p1, p2, 2.0f, WHITE);
    }
}

void render_velocity(const Body *body, float zoom, Vector2 camera_offset) {
    Vector2 start = vec2_vadd(vec2_vscale(body->position, zoom), camera_offset);
    float scale = 20.0f;
    Vector2 vel_scaled = vec2_vscale(body->velocity, scale);
    Vector2 end_world = vec2_vadd(body->position, vel_scaled);
    Vector2 end = vec2_vadd(vec2_vscale(end_world, zoom), camera_offset);

    DrawLineEx(start, end, 2.0f, RED);
}
