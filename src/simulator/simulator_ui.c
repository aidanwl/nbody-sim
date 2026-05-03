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
        sim->camera_pan = (Vector2){0.0f, 0.0f};
        sim->locked_body_index = -1;
        sim->zoom = 1.0f;
    }
}

static void simulator_draw_body_lock_menu(Simulator *sim, Body bodies[], int body_count) {
    Rectangle menu_button = layout_anchor(140, 40, LAYOUT_TOP_LEFT, 20, 20);

    if (widget_button(menu_button, "Body Lock")) {
        sim->body_menu_open = !sim->body_menu_open;
    }

    if (sim->locked_body_index >= 0) {
        if (widget_button(layout_anchor(140, 40, LAYOUT_TOP_LEFT, 170, 20), "Unlock")) {
            sim->locked_body_index = -1;
        }
    }

    if (!sim->body_menu_open) {
        return;
    }

    float panel_height = 50.0f + body_count * 34.0f;
    Rectangle panel = layout_anchor(240, panel_height, LAYOUT_TOP_LEFT, 170, 70);

    DrawRectangleRec(panel, (Color){30, 30, 30, 230});
    DrawRectangleLinesEx(panel, 2.0f, WHITE);
    DrawText("Bodies", (int)(panel.x + 10), (int)(panel.y + 10), 20, WHITE);

    for (int i = 0; i < body_count; i++) {
        Rectangle button = {
            panel.x + 10.0f,
            panel.y + 40.0f + i * 34.0f,
            panel.width - 20.0f,
            28.0f
        };
        const char *label = TextFormat("Body %d  m=%.1f", i + 1, bodies[i].mass);

        if (widget_button(button, label)) {
            sim->locked_body_index = i;
            sim->camera_focus = bodies[i].position;
            sim->camera_pan = (Vector2){0.0f, 0.0f};
            sim->body_menu_open = false;
        }
    }
}

void simulator_draw_controls(Simulator *sim, Body bodies[], float *sim_speed, bool *paused, int body_count) {
    *paused = widget_toggle(
        layout_anchor(140, 40, LAYOUT_TOP_LEFT, 20, 80),
        "Paused",
        *paused
    );

    simulator_draw_options(sim);
    simulator_draw_speed_control(sim, sim_speed);
    simulator_draw_navigation(sim);
    simulator_draw_body_lock_menu(sim, bodies, body_count);

    DrawText(TextFormat("Speed: %.2fx", *sim_speed), 20, 140, 20, WHITE);
    DrawText(TextFormat("Bodies: %d", body_count), 20, 170, 20, WHITE);
}
