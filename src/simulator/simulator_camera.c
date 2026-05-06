#include "simulator/simulator.h"

#include "raylib.h"

// Move camera around with WASD and mouse
void simulator_update_camera(Simulator *sim, float frame_dt) {
    const float pan_speed = 300.0f * frame_dt;
    const float zoom_speed = 0.25f;

    if (sim->locked_body_index < 0 && !sim->input_blocked) {
        // Keyboard and right-mouse drag change screen-space pan when not following a body.
        if (IsKeyDown(KEY_W)) {
            sim->camera_pan.y += pan_speed;
        }
        if (IsKeyDown(KEY_S)) {
            sim->camera_pan.y -= pan_speed;
        }
        if (IsKeyDown(KEY_A)) {
            sim->camera_pan.x += pan_speed;
        }
        if (IsKeyDown(KEY_D)) {
            sim->camera_pan.x -= pan_speed;
        }

        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            Vector2 delta = GetMouseDelta();
            sim->camera_pan = vec2_vadd(sim->camera_pan, delta);
        }
    }

    // Block mouse wheel zoom while typing in editor/save inputs.
    float wheel = sim->input_blocked ? 0.0f : GetMouseWheelMove();

    if (wheel > 0.0f) {
        // Multiplicative zoom feels consistent at different zoom levels.
        sim->zoom *= 1.0f + zoom_speed * wheel;
    } else if (wheel < 0.0f) {
        sim->zoom /= 1.0f + zoom_speed * -wheel;
    }

    // Clamp zoom so the scene does not become unusably tiny or huge.
    if (sim->zoom < 0.1f) {
        sim->zoom = 0.1f;
    } else if (sim->zoom > 10.0f) {
        sim->zoom = 10.0f;
    }
}
