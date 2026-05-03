#include "simulator/simulator.h"

#include "core/widget.h"
#include "core/layout.h"
#include "raylib.h"

static void simulator_draw_options(Simulator *sim) {
    Rectangle panel = layout_anchor(220, 170, LAYOUT_TOP_RIGHT, 20, 120);

    DrawRectangleLines(panel.x, panel.y, panel.width, panel.height, WHITE);
    DrawText("Options", panel.x + 10, panel.y + 10, 20, WHITE);

    sim->show_paths = widget_toggle(
        layout_relative(panel, 0.05f, 0.30f, 0.90f, 0.18f),
        "Show Paths",
        sim->show_paths
    );

    sim->show_current_trajectory = widget_toggle(
        layout_relative(panel, 0.05f, 0.55f, 0.90f, 0.18f),
        "Show Current Trajectory",
        sim->show_current_trajectory
    );
}

static void simulator_draw_speed_control(Simulator *sim, float *sim_speed) {
    Rectangle panel = layout_anchor(320, 110, LAYOUT_BOTTOM_RIGHT, 20, 20);

    if (widget_button(layout_relative(panel, 0.00f, 0.00f, 0.75f, 0.28f), "Speed Control")) {
        sim->speed_slider_open = !sim->speed_slider_open;
    }

    if (sim->speed_slider_open) {
        *sim_speed = widget_slider(
            layout_relative(panel, 0.00f, 0.35f, 0.75f, 0.18f),
            0.0f,
            20.0f,
            *sim_speed,
            "Sim Speed"
        );
    }

    float y = 0.70f;
    float w = 0.14f;
    float h = 0.27f;
    float gap = 0.015f;

    if (widget_button(layout_relative(panel, 0.00f + 0 * (w + gap), y, w, h), "x0.5")) {
        *sim_speed = 0.5f;
    }
    if (widget_button(layout_relative(panel, 0.00f + 1 * (w + gap), y, w, h), "x1")) {
        *sim_speed = 1.0f;
    }
    if (widget_button(layout_relative(panel, 0.00f + 2 * (w + gap), y, w, h), "x2")) {
        *sim_speed = 2.0f;
    }
    if (widget_button(layout_relative(panel, 0.00f + 3 * (w + gap), y, w, h), "x5")) {
        *sim_speed = 5.0f;
    }
    if (widget_button(layout_relative(panel, 0.00f + 4 * (w + gap), y, w, h), "x20")) {
        *sim_speed = 20.0f;
    }
}

static void simulator_draw_navigation(Simulator *sim) {
    Rectangle origin_button = layout_anchor(30, 30, LAYOUT_BOTTOM_LEFT, 20, 20);

    if (widget_image_button(origin_button, sim->origin_icon)) {
        sim->camera_offset = (Vector2){0.0f, 0.0f};
        sim->zoom = 1.0f;
    }
}

void simulator_draw_controls(Simulator *sim, float *sim_speed, bool *paused, int body_count) {
    *paused = widget_toggle(
        layout_anchor(140, 40, LAYOUT_TOP_LEFT, 20, 80),
        "Paused",
        *paused
    );

    simulator_draw_options(sim);
    simulator_draw_speed_control(sim, sim_speed);
    simulator_draw_navigation(sim);

    DrawText(TextFormat("Speed: %.2fx", *sim_speed), 20, 140, 20, WHITE);
    DrawText(TextFormat("Bodies: %d", body_count), 20, 170, 20, WHITE);
}