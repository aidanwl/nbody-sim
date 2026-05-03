#include "simulator/simulator.h"

#include "core/renderer.h"
#include "core/widget.h"

static Vector2 world_to_screen(const Simulator *sim, Vector2 world_pos) {
    Vector2 screen_center = {
        GetScreenWidth() * 0.5f,
        GetScreenHeight() * 0.5f
    };

    Vector2 relative = vec2_vsub(world_pos, sim->camera_focus);
    Vector2 scaled = vec2_vscale(relative, sim->zoom);

    return vec2_vadd(vec2_vadd(screen_center, scaled), sim->camera_pan);
}


static void simulator_draw_trail(const Simulator *sim, const Body *body) {
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

        Vector2 p1 = world_to_screen(sim, body->trail[a]);
        Vector2 p2 = world_to_screen(sim, body->trail[b]);

        DrawLineEx(p1, p2, 2.0f, WHITE);
    }
}

static void simulator_draw_velocity(const Simulator *sim, const Body *body) {
    Vector2 start = world_to_screen(sim, body->position);

    float scale = 20.0f;
    Vector2 end_world = vec2_vadd(body->position, vec2_vscale(body->velocity, scale));
    Vector2 end = world_to_screen(sim, end_world);

    DrawLineEx(start, end, 2.0f, RED);
}

static void simulator_draw_bodies(const Simulator *sim, Body bodies[], int body_count) {
    for (int i = 0; i < body_count; i++) {
        Vector2 screen_pos = world_to_screen(sim, bodies[i].position);
        Color color = (i == sim->locked_body_index) ? YELLOW : WHITE;

        render_body(screen_pos, 5.0f * sim->zoom, color);
    }
}

static void simulator_update_body_lock(Simulator *sim, Body bodies[], int body_count) {
    if (sim->locked_body_index >= body_count) {
        sim->locked_body_index = -1;
    }

    if (sim->locked_body_index < 0) {
        return;
    }

    const float follow_strength = 0.08f;
    Vector2 target = bodies[sim->locked_body_index].position;
    Vector2 to_target = vec2_vsub(target, sim->camera_focus);

    sim->camera_focus = vec2_vadd(
        sim->camera_focus,
        vec2_vscale(to_target, follow_strength)
    );
    sim->camera_pan = (Vector2){0.0f, 0.0f};
}

void simulator_init(Simulator *sim) {
    sim->camera_focus = (Vector2){400.0f, 300.0f};
    sim->camera_pan = (Vector2){0.0f, 0.0f};
    sim->zoom = 1.0f;
    sim->origin_icon = LoadTexture("../assets/origin.png");

    sim->show_paths = false;
    sim->show_current_trajectory = false;

    sim->speed_slider_open = false;
    sim->body_menu_open = false;
    sim->locked_body_index = -1;
}

void simulator_update(Simulator *sim, float frame_dt) {
    simulator_update_camera(sim, frame_dt);
}

void simulator_draw(Simulator *sim, Body bodies[], int body_count, float *sim_speed, bool *paused) {
    simulator_update_body_lock(sim, bodies, body_count);

    if (sim->show_paths || sim->show_current_trajectory) {
        for (int i = 0; i < body_count; i++) {
            if (sim->show_paths) {
                simulator_draw_trail(sim, &bodies[i]);
            }   
            if (sim->show_current_trajectory) {
                simulator_draw_velocity(sim, &bodies[i]);
            }
            
        }
    }
    
    simulator_draw_bodies(sim, bodies, body_count);
    simulator_draw_controls(sim, bodies, sim_speed, paused, body_count);
}

void simulator_deinit(Simulator *sim) {
    UnloadTexture(sim->origin_icon);
}
