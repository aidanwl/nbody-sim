#include "simulator/simulator.h"

#include "core/renderer.h"
#include "core/widget.h"

static Vector2 world_to_screen(const Simulator *sim, Vector2 world_pos) {
    return vec2_vadd(vec2_vscale(world_pos, sim->zoom), sim->camera_offset);
}

static void simulator_draw_bodies(const Simulator *sim, Body bodies[], int body_count) {
    for (int i = 0; i < body_count; i++) {
        Vector2 screen_pos = world_to_screen(sim, bodies[i].position);
        render_body(screen_pos, 5.0f * sim->zoom, WHITE);
    }
}

void simulator_init(Simulator *sim) {
    sim->camera_offset = (Vector2){0.0f, 0.0f};
    sim->zoom = 1.0f;
    sim->origin_icon = LoadTexture("../assets/origin.png");

    sim->show_paths = false;
    // sim->show_current_trajectory = false;

    sim->speed_slider_open = false;
}

void simulator_update(Simulator *sim, float frame_dt) {
    simulator_update_camera(sim, frame_dt);
}

void simulator_draw(Simulator *sim, Body bodies[], int body_count, float *sim_speed, bool *paused) {

    if (sim->show_paths) {
        for (int i = 0; i < body_count; i++) {
            render_trail(&bodies[i], sim->zoom, sim->camera_offset);
        }
    }
    

    simulator_draw_bodies(sim, bodies, body_count);
    simulator_draw_controls(sim, sim_speed, paused, body_count);
}
