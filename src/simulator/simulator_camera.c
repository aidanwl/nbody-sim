#include "simulator/simulator.h"

#include "raylib.h"

void simulator_update_camera(Simulator *sim, float frame_dt) {
    const float pan_speed = 300.0f * frame_dt;
    const float zoom_speed = 1.0f;

    if (IsKeyDown(KEY_W)) {
        sim->camera_offset.y += pan_speed;
    }
    if (IsKeyDown(KEY_S)) {
        sim->camera_offset.y -= pan_speed;
    }
    if (IsKeyDown(KEY_A)) {
        sim->camera_offset.x += pan_speed;
    }
    if (IsKeyDown(KEY_D)) {
        sim->camera_offset.x -= pan_speed;
    }

    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        Vector2 delta = GetMouseDelta();
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            Vector2 delta = GetMouseDelta();
            sim->camera_pan = vec2_vadd(sim->camera_pan, delta);
            sim->camera_user_moved = true;
        }
    }

    float wheel = GetMouseWheelMove();

    if (wheel > 0.0f) {
        sim->zoom *= (1.0f + zoom_speed * frame_dt);
    } else if (wheel < 0.0f) {
        sim->zoom /= (1.0f + zoom_speed * frame_dt);
    }

    if (sim->zoom < 0.1f) {
        sim->zoom = 0.1f;
    } else if (sim->zoom > 10.0f) {
        sim->zoom = 10.0f;
    }

}