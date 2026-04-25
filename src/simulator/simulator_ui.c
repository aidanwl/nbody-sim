#include "simulator/simulator.h"

#include "core/widget.h"
#include "raylib.h"

static void simulator_draw_options(Simulator *sim) {
    DrawRectangleLines(610, 140, 170, 160, WHITE);
    DrawText("Options", 620, 150, 20, WHITE);

    sim->show_paths = widget_toggle(
        (Rectangle){620, 190, 140, 30},
        "Show Paths",
        sim->show_paths
    ); // Doesn't do anything yet

    sim->show_current_trajectory = widget_toggle(
        (Rectangle){620, 220, 140, 30},
        "Show Current Trajectory",
        sim->show_current_trajectory
    ); // Doesn't do anything yet

}

static void simulator_draw_speed_control(Simulator *sim, float *sim_speed) {

    // Button Controls
    if (widget_button((Rectangle){560, 530, 45, 30}, "x0.5")) {
        *sim_speed = 0.5f;
    }
    if (widget_button((Rectangle){610, 530, 45, 30}, "x1")) {
        *sim_speed = 1.0f;
    }
    if (widget_button((Rectangle){660, 530, 45, 30}, "x2")) {
        *sim_speed = 2.0f;
    }
    if (widget_button((Rectangle){710, 530, 45, 30}, "x5")) {
        *sim_speed = 5.0f;
    }
    if (widget_button((Rectangle){760, 530, 45, 30}, "x20")) {
        *sim_speed = 20.0f;
    }

    // Slider Control
    if (widget_button((Rectangle){560, 480, 240, 30}, "Speed Control")) {
        sim->speed_slider_open = !sim->speed_slider_open;
    }

    if (sim->speed_slider_open) {
        *sim_speed = widget_slider(
            (Rectangle){500, 500, 200, 20},
            0.0f,
            20.0f,
            *sim_speed,
            "Sim Speed"
        );
    }

}

static void simulator_draw_navigation(Simulator *sim) {
    if (widget_image_button((Rectangle){20, 560, 30, 30}, sim->origin_icon)) {
        sim->camera_offset = (Vector2){0.0f, 0.0f};
        sim->zoom = 1.0f;
    }
}

void simulator_draw_controls(Simulator *sim, float *sim_speed, bool *paused, int body_count) {
    // simulator_draw_mode_selector();

    *paused = widget_toggle((Rectangle){20, 80, 140, 40}, "Paused", *paused);

    simulator_draw_options(sim);
    simulator_draw_speed_control(sim, sim_speed);
    simulator_draw_navigation(sim);

    DrawText(TextFormat("Speed: %.2fx", *sim_speed), 20, 140, 20, WHITE);
    DrawText(TextFormat("Bodies: %d", body_count), 20, 170, 20, WHITE);
}