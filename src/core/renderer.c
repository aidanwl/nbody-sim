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

static Vector2 camera_world_to_screen(Vector2 world_pos, float zoom, Vector2 camera_focus, Vector2 camera_pan) {
    Vector2 screen_center = {
        GetScreenWidth() * 0.5f,
        GetScreenHeight() * 0.5f
    };

    Vector2 relative = vec2_vsub(world_pos, camera_focus);
    Vector2 scaled = vec2_vscale(relative, zoom);

    return vec2_vadd(vec2_vadd(screen_center, scaled), camera_pan);
}


void render_trail(const Body *body, float zoom, Vector2 camera_focus, Vector2 camera_pan) {
    int step = 1;

    if (body->trail_count > 2000) {
        step = 8;
    } else if (body->trail_count > 1000) {
        step = 4;
    } else if (body->trail_count > 500) {
        step = 2;
    }

    for (int i = step; i < body->trail_count; i += step) {
        int a = (body->trail_start + i - step) % TRAIL_MAX;
        int b = (body->trail_start + i) % TRAIL_MAX;

        Vector2 p1 = camera_world_to_screen(body->trail[a], zoom, camera_focus, camera_pan);
        Vector2 p2 = camera_world_to_screen(body->trail[b], zoom, camera_focus, camera_pan);

        DrawLineEx(p1, p2, 2.0f, WHITE);
    }
}

void render_velocity(const Body *body, float zoom, Vector2 camera_focus, Vector2 camera_pan) {
    Vector2 start = camera_world_to_screen(body->position, zoom, camera_focus, camera_pan);

    float scale = 20.0f;
    Vector2 end_world = vec2_vadd(body->position, vec2_vscale(body->velocity, scale));
    Vector2 end = camera_world_to_screen(end_world, zoom, camera_focus, camera_pan);

    DrawLineEx(start, end, 2.0f, RED);
}