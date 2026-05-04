#include "simulator/simulator.h"
#include "simulator/simulator_templates.h"

#include <stddef.h>

#include "core/widget.h"
#include "core/layout.h"
#include "raylib.h"

static void simulator_draw_options(Simulator *sim) {
    Rectangle panel = layout_anchor(220, 170, LAYOUT_TOP_RIGHT, 20, 120);
    const char *path_label = "Paths: Off";

    switch (sim->path_mode) {
        case PATH_MODE_SHORT:
            path_label = "Paths: Short";
            break;
        case PATH_MODE_LONG:
            path_label = "Paths: Long";
            break;
        case PATH_MODE_ORBIT:
            path_label = "Paths: Orbit";
            break;
        case PATH_MODE_OFF:
        default:
            path_label = "Paths: Off";
            break;
    }

    DrawRectangleLines(panel.x, panel.y, panel.width, panel.height, WHITE);
    DrawText("Options", panel.x + 10, panel.y + 10, 20, WHITE);

    if (widget_button(
        layout_relative(panel, 0.05f, 0.30f, 0.90f, 0.18f),
        path_label
    )) {
        sim->path_mode = (PathMode)((sim->path_mode + 1) % 4);
    }

    sim->show_current_trajectory = widget_toggle(
        layout_relative(panel, 0.05f, 0.55f, 0.90f, 0.18f),
        "Velocity Vectors",
        sim->show_current_trajectory
    );
}

static void simulator_draw_speed_control(Simulator *sim, float *sim_speed) {
    Rectangle button = layout_anchor(86, 34, LAYOUT_BOTTOM_RIGHT, 20, 20);
    Rectangle presets = layout_anchor(230, 30, LAYOUT_BOTTOM_RIGHT, 116, 22);
    Rectangle slider_panel = layout_anchor(230, 74, LAYOUT_BOTTOM_RIGHT, 20, 62);
    const float preset_values[] = {0.5f, 1.0f, 2.0f, 5.0f, 20.0f};
    const char *preset_labels[] = {"x.5", "x1", "x2", "x5", "x20"};

    if (widget_button(button, "Speed")) {
        sim->speed_slider_open = !sim->speed_slider_open;
    }

    if (sim->speed_slider_open) {
        DrawRectangleRec(slider_panel, (Color){30, 30, 30, 220});
        DrawRectangleLinesEx(slider_panel, 2.0f, WHITE);

        *sim_speed = widget_slider(
            layout_relative(slider_panel, 0.08f, 0.55f, 0.55f, 0.20f),
            0.0f,
            20.0f,
            *sim_speed,
            "Speed"
        );
    }

    const float w = 0.18f;
    const float gap = 0.025f;
    const int preset_count = (int)(sizeof(preset_values) / sizeof(preset_values[0]));

    for (int i = 0; i < preset_count; i++) {
        Rectangle preset_button = layout_relative(presets, i * (w + gap), 0.0f, w, 1.0f);

        if (widget_button(preset_button, preset_labels[i])) {
            *sim_speed = preset_values[i];
        }
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

static void simulator_draw_templates_menu(Simulator *sim) {
    Rectangle menu_button = layout_anchor(140, 40, LAYOUT_TOP_RIGHT, 20, 20);
    Rectangle reset_button = layout_anchor(140, 40, LAYOUT_TOP_RIGHT, 20, 70);

    if (widget_button(menu_button, "Templates")) {
        sim->template_menu_open = !sim->template_menu_open;
        sim->body_menu_open = false;
    }

    if (widget_button(reset_button, "Reset")) {
        sim->requested_template_index = sim->active_template_index;
        sim->template_menu_open = false;
    }

    if (!sim->template_menu_open) {
        return;
    }

    int template_count = simulator_template_count();
    Rectangle panel = layout_anchor(240, 50.0f + template_count * 34.0f, LAYOUT_TOP_RIGHT, 20, 120);

    DrawRectangleRec(panel, (Color){30, 30, 30, 230});
    DrawRectangleLinesEx(panel, 2.0f, WHITE);
    DrawText("Templates", (int)(panel.x + 10), (int)(panel.y + 10), 20, WHITE);

    for (int i = 0; i < template_count; i++) {
        const SimulatorTemplate *template = simulator_template_get(i);
        const char *label = template == NULL ? "Unknown" : template->name;
        Rectangle button = {
            panel.x + 10.0f,
            panel.y + 40.0f + i * 34.0f,
            panel.width - 20.0f,
            28.0f
        };

        if (i == sim->active_template_index) {
            DrawRectangleLinesEx(
                (Rectangle){button.x - 3.0f, button.y - 3.0f, button.width + 6.0f, button.height + 6.0f},
                2.0f,
                GREEN
            );
        }

        if (widget_button(button, label)) {
            sim->requested_template_index = i;
            sim->template_menu_open = false;
        }
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
    Rectangle panel = layout_anchor(240, panel_height, LAYOUT_TOP_LEFT, 170, 20);

    DrawRectangleRec(panel, (Color){30, 30, 30, 230});
    DrawRectangleLinesEx(panel, 2.0f, WHITE);
    DrawText("Bodies", (int)(panel.x + 10), (int)(panel.y + 10), 20, WHITE);

    for (int i = 0; i < body_count; i++) {
        Rectangle lock_button = {
            panel.x + 10.0f,
            panel.y + 40.0f + i * 34.0f,
            panel.width - 64.0f,
            28.0f
        };
        Rectangle delete_button = {
            panel.x + panel.width - 48.0f,
            lock_button.y,
            38.0f,
            lock_button.height
        };
        const char *label = TextFormat("%s  m=%.1f", bodies[i].name, bodies[i].mass);

        if (widget_button(lock_button, label)) {
            sim->locked_body_index = i;
            sim->camera_focus = bodies[i].position;
            sim->camera_pan = (Vector2){0.0f, 0.0f};
            sim->body_menu_open = false;
        }

        if (widget_button(delete_button, "Del")) {
            sim->delete_body_index = i;
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
    simulator_draw_templates_menu(sim);
    simulator_draw_speed_control(sim, sim_speed);
    simulator_draw_navigation(sim);
    simulator_draw_body_lock_menu(sim, bodies, body_count);

    DrawText(TextFormat("Speed: %.2fx", *sim_speed), 20, 140, 20, WHITE);
    DrawText(TextFormat("Bodies: %d", body_count), 20, 170, 20, WHITE);
}
