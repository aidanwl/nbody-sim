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
    if (widget_button((Rectangle){560, 530, 45, 30}, "0.5x")) {
        *sim_speed = 0.5f;
    }
    if (widget_button((Rectangle){610, 530, 45, 30}, "1x")) {
        *sim_speed = 1.0f;
    }
    if (widget_button((Rectangle){660, 530, 45, 30}, "2x")) {
        *sim_speed = 2.0f;
    }
    if (widget_button((Rectangle){710, 530, 45, 30}, "5x")) {
        *sim_speed = 5.0f;
    }
    if (widget_button((Rectangle){760, 530, 45, 30}, "20x")) {
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