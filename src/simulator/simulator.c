#include "simulator/simulator.h"

#include "core/renderer.h"
#include "core/widget.h"

const Vector2 SIMULATOR_ORIGIN = {400.0f, 300.0f};

// Converts world coordinates to pixel coordinates
Vector2 simulator_world_to_screen(const Simulator *sim, Vector2 world_pos) {
    // The camera focus is drawn at the center of the current window.
    Vector2 screen_center = {
        GetScreenWidth() * 0.5f,
        GetScreenHeight() * 0.5f
    };

    Vector2 relative = vec2_vsub(world_pos, sim->camera_focus);
    // Zoom scales distance from the camera focus, then pan shifts the final screen position.
    Vector2 scaled = vec2_vscale(relative, sim->zoom);

    return vec2_vadd(vec2_vadd(screen_center, scaled), sim->camera_pan);
}

// Converts screen coordinates to world coordinates
Vector2 simulator_screen_to_world(const Simulator *sim, Vector2 screen_pos) {
    Vector2 screen_center = {
        GetScreenWidth() * 0.5f,
        GetScreenHeight() * 0.5f
    };
    // Undo pan and zoom in the opposite order used by simulator_world_to_screen.
    Vector2 unpanned = vec2_vsub(vec2_vsub(screen_pos, screen_center), sim->camera_pan);
    Vector2 relative = vec2_vscale(unpanned, 1.0f / sim->zoom);

    return vec2_vadd(relative, sim->camera_focus);
}

// Centers the camera on a world-space point and clears body-follow mode.
void simulator_center_on_world(Simulator *sim, Vector2 world_pos) {
    sim->locked_body_index = -1;
    sim->camera_focus = world_pos;
    sim->camera_pan = (Vector2){0.0f, 0.0f};
}

// Converts the selected path mode into the maximum trail points to draw.
static int simulator_path_point_limit(PathMode mode) {
    switch (mode) {
        case PATH_MODE_SHORT:
            return 600;
        case PATH_MODE_LONG:
            return 2500;
        case PATH_MODE_ORBIT:
            return TRAIL_MAX;
        case PATH_MODE_OFF:
        default:
            return 0;
    }
}

// Draws one body's stored trail history using the circular trail buffer.
static void simulator_draw_trail(const Simulator *sim, const Body *body, Color color) {
    int point_limit = simulator_path_point_limit(sim->path_mode);

    if (point_limit <= 0) {
        return;
    }

    // Limit visible samples so Short/Long modes do not draw the full history.
    int visible_count = body->trail_count;
    if (visible_count > point_limit) {
        visible_count = point_limit;
    }

    int draw_start = body->trail_count - visible_count;
    int step = 1;

    // Skip points for very long trails to keep rendering affordable.
    if (visible_count > 8000) {
        step = 12;
    } else if (visible_count > 4000) {
        step = 6;
    } else if (visible_count > 1500) {
        step = 3;
    }

    for (int i = draw_start + step; i < body->trail_count; i += step) {
        // trail_start makes the array act like a circular buffer.
        int a = (body->trail_start + i - step) % TRAIL_MAX;
        int b = (body->trail_start + i) % TRAIL_MAX;

        Vector2 p1 = simulator_world_to_screen(sim, body->trail[a]);
        Vector2 p2 = simulator_world_to_screen(sim, body->trail[b]);

        DrawLineEx(p1, p2, 2.0f, color);
    }
}

// Draws a red velocity vector from the body's current position.
static void simulator_draw_velocity(const Simulator *sim, const Body *body) {
    Vector2 start = simulator_world_to_screen(sim, body->position);

    // Scale velocity so the vector is visible in screen space.
    float scale = 20.0f;
    Vector2 end_world = vec2_vadd(body->position, vec2_vscale(body->velocity, scale));
    Vector2 end = simulator_world_to_screen(sim, end_world);

    DrawLineEx(start, end, 2.0f, RED);
}

// Draws all bodies after converting their world positions to screen positions.
static void simulator_draw_bodies(const Simulator *sim, Body bodies[], int body_count) {
    for (int i = 0; i < body_count; i++) {
        Vector2 screen_pos = simulator_world_to_screen(sim, bodies[i].position);
        Color color = bodies[i].color;

        render_body(screen_pos, 5.0f * sim->zoom, color);
    }
}

// Updates which body name label should be shown based on mouse clicks.
static void simulator_update_body_name_selection(Simulator *sim, Body bodies[], int body_count) {
    if (sim->named_body_index >= body_count) {
        sim->named_body_index = -1;
    }

    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return;
    }

    Vector2 mouse = GetMousePosition();
    int clicked_body = -1;

    // Iterate backward so later-drawn bodies are selected first when circles overlap.
    for (int i = body_count - 1; i >= 0; i--) {
        Vector2 screen_pos = simulator_world_to_screen(sim, bodies[i].position);
        float radius = 5.0f * sim->zoom;

        if (CheckCollisionPointCircle(mouse, screen_pos, radius + 4.0f)) {
            clicked_body = i;
            break;
        }
    }

    sim->named_body_index = clicked_body;
}

// Draws the selected body's name above it.
static void simulator_draw_body_name(const Simulator *sim, Body bodies[], int body_count) {
    if (sim->named_body_index < 0 || sim->named_body_index >= body_count) {
        return;
    }

    const Body *body = &bodies[sim->named_body_index];
    Vector2 screen_pos = simulator_world_to_screen(sim, body->position);
    int font_size = 20;
    int text_width = MeasureText(body->name, font_size);

    DrawText(
        body->name,
        (int)(screen_pos.x - text_width * 0.5f),
        (int)(screen_pos.y - 34.0f),
        font_size,
        body->color
    );
}

// Smoothly follows the locked body by moving camera_focus toward it each frame.
static void simulator_update_body_lock(Simulator *sim, Body bodies[], int body_count) {
    if (sim->locked_body_index >= body_count) {
        sim->locked_body_index = -1;
    }

    if (sim->locked_body_index < 0) {
        return;
    }

    // Strength below 1.0 creates smooth camera easing instead of snapping.
    const float follow_strength = 0.08f;
    Vector2 target = bodies[sim->locked_body_index].position;
    Vector2 to_target = vec2_vsub(target, sim->camera_focus);

    sim->camera_focus = vec2_vadd(
        sim->camera_focus,
        vec2_vscale(to_target, follow_strength)
    );
    sim->camera_pan = (Vector2){0.0f, 0.0f};
}

// Restores simulator camera, menus, and request flags to their default state.
void simulator_reset(Simulator *sim) {
    // Reset home to the fixed simulation origin, not wherever body-lock last focused.
    sim->camera_focus = SIMULATOR_ORIGIN;
    sim->camera_pan = (Vector2){0.0f, 0.0f};
    sim->zoom = 1.0f;

    sim->path_mode = PATH_MODE_OFF;
    sim->show_current_trajectory = false;

    sim->speed_slider_open = false;
    sim->body_menu_open = false;
    sim->template_menu_open = false;
    sim->advanced_menu_open = false;
    sim->input_blocked = false;
    sim->controls_blocked = false;
    sim->save_system_requested = false;
    sim->save_prompt_open = false;
    sim->requested_template_index = -1;
    sim->requested_saved_system_index = -1;
    sim->delete_saved_system_index = -1;
    sim->locked_body_index = -1;
    sim->named_body_index = -1;
    sim->stats_body_index = -1;
    sim->delete_body_index = -1;
    sim->edit_body_index = -1;
    sim->save_system_name[0] = '\0';
}

// Initializes simulator resources and default UI state.
void simulator_init(Simulator *sim) {
    // Try both runtime paths because the executable may run from build/ or repo root.
    sim->origin_icon = LoadTexture(FileExists("assets/origin.png") ? "assets/origin.png" : "../assets/origin.png");
    sim->active_template_index = 0;
    sim->saved_system_count = 0;
    simulator_reset(sim);
}

// Per-frame simulator update for camera movement.
void simulator_update(Simulator *sim, float frame_dt) {
    simulator_update_camera(sim, frame_dt);
}

// Draws the physics scene before drawing controls.
static void simulator_draw_scene(Simulator *sim, Body bodies[], int body_count) {
    // Follow and selection are updated during draw because they depend on current bodies/mouse.
    simulator_update_body_lock(sim, bodies, body_count);
    simulator_update_body_name_selection(sim, bodies, body_count);

    if (sim->path_mode != PATH_MODE_OFF || sim->show_current_trajectory) {
        for (int i = 0; i < body_count; i++) {
            if (sim->path_mode != PATH_MODE_OFF) {
                simulator_draw_trail(sim, &bodies[i], bodies[i].color);
            }   
            if (sim->show_current_trajectory) {
                simulator_draw_velocity(sim, &bodies[i]);
            }
            
        }
    }
    
    simulator_draw_bodies(sim, bodies, body_count);
    simulator_draw_body_name(sim, bodies, body_count);
}

// Main simulator draw entry: scene first, UI controls second.
void simulator_draw(Simulator *sim, Body bodies[], int body_count, float *sim_speed, bool *paused, double sim_time_seconds) {
    simulator_draw_scene(sim, bodies, body_count);
    simulator_draw_controls(sim, bodies, sim_speed, paused, body_count, sim_time_seconds);
}

// Frees textures loaded by simulator_init.
void simulator_deinit(Simulator *sim) {
    UnloadTexture(sim->origin_icon);
}
