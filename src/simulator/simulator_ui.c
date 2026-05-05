#include "simulator/simulator.h"
#include "simulator/simulator_templates.h"

#include <stddef.h>
#include <string.h>

#include "core/widget.h"
#include "core/layout.h"
#include "raylib.h"

#define SIM_UI_LEFT_MARGIN 20.0f
#define SIM_UI_SPEED_COUNTER_Y 20.0f
#define SIM_UI_BODY_COUNT_Y 46.0f
#define SIM_UI_PAUSE_Y 80.0f
#define SIM_UI_BODY_LOCK_Y 130.0f
#define SIM_UI_RIGHT_MARGIN 20.0f
#define SIM_UI_TIME_Y 20.0f
#define SIM_UI_OPTIONS_Y 62.0f
#define SIM_UI_ADVANCED_Y 202.0f
#define SIM_UI_ADVANCED_PANEL_Y 252.0f

static void simulator_draw_panel(Rectangle panel, const char *title) {
    DrawRectangleRec(panel, (Color){30, 30, 30, 230});
    DrawRectangleLinesEx(panel, 2.0f, WHITE);
    DrawText(title, (int)(panel.x + 10), (int)(panel.y + 10), 20, WHITE);
}

static Rectangle simulator_panel_row(Rectangle panel, int index, float width) {
    return (Rectangle){
        panel.x + 10.0f,
        panel.y + 40.0f + index * 34.0f,
        width,
        28.0f
    };
}

static void simulator_draw_options(Simulator *sim) {
    Rectangle panel = layout_anchor(220, 130, LAYOUT_TOP_RIGHT, SIM_UI_RIGHT_MARGIN, SIM_UI_OPTIONS_Y);
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
        layout_relative(panel, 0.05f, 0.34f, 0.90f, 0.22f),
        path_label
    )) {
        sim->path_mode = (PathMode)((sim->path_mode + 1) % 4);
    }

    sim->show_current_trajectory = widget_toggle(
        layout_relative(panel, 0.05f, 0.64f, 0.90f, 0.22f),
        "Velocity Vectors",
        sim->show_current_trajectory
    );
}

static void simulator_draw_time_display(double sim_time_seconds) {
    long long total_seconds = (long long)sim_time_seconds;
    long long hours = total_seconds / 3600;
    long long minutes = (total_seconds / 60) % 60;
    long long seconds = total_seconds % 60;
    Rectangle panel = layout_anchor(220, 34, LAYOUT_TOP_RIGHT, SIM_UI_RIGHT_MARGIN, SIM_UI_TIME_Y);

    DrawRectangleRec(panel, (Color){30, 30, 30, 210});
    DrawRectangleLinesEx(panel, 2.0f, WHITE);
    DrawText(
        TextFormat("Time: %02lld:%02lld:%02lld", hours, minutes, seconds),
        (int)(panel.x + 10),
        (int)(panel.y + 7),
        20,
        WHITE
    );
}

static void simulator_draw_speed_control(Simulator *sim, float *sim_speed) {
    Rectangle button = layout_anchor(86, 34, LAYOUT_BOTTOM_RIGHT, 20, 20);
    Rectangle presets = layout_anchor(248, 30, LAYOUT_BOTTOM_RIGHT, 62, 22);
    Rectangle slider_panel = layout_anchor(260, 74, LAYOUT_BOTTOM_RIGHT, 20, 62);
    const float preset_values[] = {1.0f, 10.0f, 25.0f, 100.0f};
    const char *preset_labels[] = {"x1", "x10", "x25", "x100"};
    const int preset_keys[] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR};
    const int preset_keypad_keys[] = {KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_4};

    if (widget_button(button, "Speed")) {
        sim->speed_slider_open = !sim->speed_slider_open;
    }

    if (sim->speed_slider_open) {
        DrawRectangleRec(slider_panel, (Color){30, 30, 30, 220});
        DrawRectangleLinesEx(slider_panel, 2.0f, WHITE);

        *sim_speed = widget_slider_format(
            layout_relative(slider_panel, 0.08f, 0.55f, 0.55f, 0.20f),
            0.0f,
            100.0f,
            *sim_speed,
            "Speed",
            "x%.1f"
        );
    }

    const float w = 0.18f;
    const float gap = 0.025f;
    const int preset_count = (int)(sizeof(preset_values) / sizeof(preset_values[0]));

    for (int i = 0; i < preset_count; i++) {
        Rectangle preset_button = layout_relative(presets, i * (w + gap), 0.0f, w, 1.0f);

        if (widget_button(preset_button, preset_labels[i]) ||
            (!sim->input_blocked && (IsKeyPressed(preset_keys[i]) || IsKeyPressed(preset_keypad_keys[i])))) {
            *sim_speed = preset_values[i];
        }
    }
}

static bool simulator_text_has_visible_char(const char *text) {
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] != ' ' && text[i] != '\t') {
            return true;
        }
    }

    return false;
}

static void simulator_update_save_name_input(Simulator *sim) {
    int len = (int)strlen(sim->save_system_name);
    int key = GetCharPressed();

    while (key > 0) {
        bool allowed = key >= 32 && key <= 126;

        if (allowed && len < (int)sizeof(sim->save_system_name) - 1) {
            sim->save_system_name[len++] = (char)key;
            sim->save_system_name[len] = '\0';
        }

        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && len > 0) {
        sim->save_system_name[len - 1] = '\0';
    }
}

static void simulator_open_save_prompt(Simulator *sim) {
    sim->save_system_name[0] = '\0';
    sim->save_prompt_open = true;
    sim->template_menu_open = false;
}

static void simulator_draw_save_prompt(Simulator *sim) {
    if (!sim->save_prompt_open) {
        return;
    }

    Rectangle panel = layout_anchor(300, 150, LAYOUT_BOTTOM_LEFT, 210, 60);
    Rectangle input = layout_relative(panel, 0.06f, 0.34f, 0.88f, 0.25f);
    Rectangle confirm = layout_relative(panel, 0.06f, 0.70f, 0.42f, 0.20f);
    Rectangle cancel = layout_relative(panel, 0.52f, 0.70f, 0.42f, 0.20f);

    simulator_update_save_name_input(sim);

    DrawRectangleRec(panel, (Color){30, 30, 30, 240});
    DrawRectangleLinesEx(panel, 2.0f, WHITE);
    DrawText("Save System", (int)(panel.x + 12), (int)(panel.y + 12), 20, WHITE);
    DrawText("Name", (int)input.x, (int)(input.y - 20), 16, WHITE);
    DrawRectangleRec(input, RAYWHITE);
    DrawRectangleLinesEx(input, 2.0f, SKYBLUE);

    int font_size = 20;
    int text_width = MeasureText(sim->save_system_name, font_size);

    while (font_size > 10 && text_width > input.width - 16.0f) {
        font_size--;
        text_width = MeasureText(sim->save_system_name, font_size);
    }

    DrawText(sim->save_system_name, (int)(input.x + 8), (int)(input.y + (input.height - font_size) * 0.5f), font_size, BLACK);

    bool can_save = simulator_text_has_visible_char(sim->save_system_name);
    bool confirm_clicked = widget_button(confirm, "Confirm");

    if (can_save && (confirm_clicked || IsKeyPressed(KEY_ENTER))) {
        sim->save_system_requested = true;
        sim->save_prompt_open = false;
        sim->template_menu_open = true;
    }

    if (widget_button(cancel, "Cancel") || IsKeyPressed(KEY_ESCAPE)) {
        sim->save_prompt_open = false;
    }
}

static void simulator_draw_navigation(Simulator *sim) {
    Rectangle origin_button = layout_anchor(30, 30, LAYOUT_BOTTOM_LEFT, 20, 20);

    if (widget_image_button(origin_button, sim->origin_icon)) {
        sim->camera_focus = SIMULATOR_ORIGIN;
        sim->camera_pan = (Vector2){0.0f, 0.0f};
        sim->locked_body_index = -1;
        sim->zoom = 1.0f;
    }
}

static void simulator_draw_templates_menu(Simulator *sim) {
    Rectangle menu_button = layout_anchor(140, 30, LAYOUT_BOTTOM_LEFT, 60, 20);
    Rectangle save_button = layout_anchor(140, 30, LAYOUT_BOTTOM_LEFT, 210, 20);

    if (widget_button(menu_button, "Templates")) {
        sim->template_menu_open = !sim->template_menu_open;
        sim->body_menu_open = false;
    }

    if (widget_button(save_button, "Save")) {
        simulator_open_save_prompt(sim);
    }

    if (!sim->template_menu_open) {
        return;
    }

    int template_count = simulator_template_count();
    int row_count = template_count + sim->saved_system_count;
    Rectangle panel = layout_anchor(260, 50.0f + row_count * 34.0f, LAYOUT_BOTTOM_LEFT, 60, 60);

    simulator_draw_panel(panel, "Templates");

    for (int i = 0; i < template_count; i++) {
        const SimulatorTemplate *template = simulator_template_get(i);
        const char *label = template == NULL ? "Unknown" : template->name;
        Rectangle button = simulator_panel_row(panel, i, panel.width - 20.0f);

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

    for (int i = 0; i < sim->saved_system_count; i++) {
        Rectangle button = simulator_panel_row(panel, template_count + i, panel.width - 20.0f);

        if (widget_button(button, sim->saved_system_names[i])) {
            sim->requested_saved_system_index = i;
            sim->template_menu_open = false;
        }
    }
}

static void simulator_draw_body_stats(const Body *body, Rectangle panel, float *cursor_y) {
    Vector2 acceleration = {0.0f, 0.0f};

    if (body->mass != 0.0f) {
        acceleration = vec2_vscale(body->force, 1.0f / body->mass);
    }

    DrawText(TextFormat("Position: %.2f, %.2f", body->position.x, body->position.y), (int)(panel.x + 18), (int)*cursor_y, 16, WHITE);
    *cursor_y += 20.0f;
    DrawText(TextFormat("Velocity: %.2f, %.2f", body->velocity.x, body->velocity.y), (int)(panel.x + 18), (int)*cursor_y, 16, WHITE);
    *cursor_y += 20.0f;
    DrawText(TextFormat("Accel: %.4f, %.4f", acceleration.x, acceleration.y), (int)(panel.x + 18), (int)*cursor_y, 16, WHITE);
    *cursor_y += 20.0f;
    DrawText(TextFormat("Mass: %.2f", body->mass), (int)(panel.x + 18), (int)*cursor_y, 16, WHITE);
    *cursor_y += 26.0f;
}

static void simulator_draw_advanced_menu(Simulator *sim, Body bodies[], int body_count) {
    Rectangle menu_button = layout_anchor(220, 40, LAYOUT_TOP_RIGHT, SIM_UI_RIGHT_MARGIN, SIM_UI_ADVANCED_Y);

    if (widget_button(menu_button, "Advanced")) {
        sim->advanced_menu_open = !sim->advanced_menu_open;
    }

    if (!sim->advanced_menu_open) {
        return;
    }

    if (sim->stats_body_index >= body_count) {
        sim->stats_body_index = -1;
    }

    float panel_height = 52.0f;

    if (body_count <= 0) {
        panel_height += 34.0f;
    }

    for (int i = 0; i < body_count; i++) {
        panel_height += 34.0f;
        if (i == sim->stats_body_index) {
            panel_height += 90.0f;
        }
    }

    Rectangle panel = layout_anchor(300, panel_height, LAYOUT_TOP_RIGHT, SIM_UI_RIGHT_MARGIN, SIM_UI_ADVANCED_PANEL_Y);
    float cursor_y = panel.y + 40.0f;

    simulator_draw_panel(panel, "Body Stats");

    if (body_count <= 0) {
        DrawText("No bodies", (int)(panel.x + 10), (int)cursor_y, 20, WHITE);
        return;
    }

    for (int i = 0; i < body_count; i++) {
        Rectangle header = {
            panel.x + 10.0f,
            cursor_y,
            panel.width - 20.0f,
            28.0f
        };
        bool was_open = i == sim->stats_body_index;
        bool is_open = widget_dropdown(header, bodies[i].name, was_open);

        if (is_open && !was_open) {
            sim->stats_body_index = i;
        } else if (!is_open && was_open) {
            sim->stats_body_index = -1;
        }

        cursor_y += 34.0f;

        if (i == sim->stats_body_index) {
            simulator_draw_body_stats(&bodies[i], panel, &cursor_y);
        }
    }
}

static void simulator_draw_body_lock_menu(Simulator *sim, Body bodies[], int body_count) {
    Rectangle menu_button = layout_anchor(220, 40, LAYOUT_TOP_LEFT, SIM_UI_LEFT_MARGIN, SIM_UI_BODY_LOCK_Y);

    if (widget_button(menu_button, "Body Lock")) {
        sim->body_menu_open = !sim->body_menu_open;
    }

    if (sim->locked_body_index >= 0) {
        if (widget_button(layout_anchor(100, 40, LAYOUT_TOP_LEFT, 250, SIM_UI_BODY_LOCK_Y), "Unlock")) {
            sim->locked_body_index = -1;
        }
    }

    if (!sim->body_menu_open) {
        return;
    }

    float panel_height = 50.0f + body_count * 34.0f;
    Rectangle panel = layout_anchor(340, panel_height, LAYOUT_TOP_LEFT, 250, SIM_UI_BODY_LOCK_Y + 50.0f);

    simulator_draw_panel(panel, "Bodies");

    for (int i = 0; i < body_count; i++) {
        Rectangle lock_button = simulator_panel_row(panel, i, panel.width - 116.0f);
        Rectangle edit_button = {
            panel.x + panel.width - 96.0f,
            lock_button.y,
            38.0f,
            lock_button.height
        };
        Rectangle delete_button = {
            panel.x + panel.width - 48.0f,
            lock_button.y,
            38.0f,
            lock_button.height
        };
        Rectangle color_chip = {
            lock_button.x + 6.0f,
            lock_button.y + 6.0f,
            16.0f,
            lock_button.height - 12.0f
        };
        Rectangle name_button = {
            lock_button.x + 28.0f,
            lock_button.y,
            lock_button.width - 28.0f,
            lock_button.height
        };
        const char *label = TextFormat("%s m=%.1f", bodies[i].name, bodies[i].mass);

        DrawRectangleRec(lock_button, RAYWHITE);
        DrawRectangleLinesEx(lock_button, 2.0f, BLACK);
        DrawRectangleRec(color_chip, bodies[i].color);
        DrawRectangleLinesEx(color_chip, 1.0f, BLACK);

        if (widget_button(name_button, label)) {
            sim->locked_body_index = i;
            sim->camera_focus = bodies[i].position;
            sim->camera_pan = (Vector2){0.0f, 0.0f};
            sim->body_menu_open = false;
        }

        if (widget_button(edit_button, "Edit")) {
            sim->edit_body_index = i;
            sim->body_menu_open = false;
        }

        if (widget_button(delete_button, "Del")) {
            sim->delete_body_index = i;
        }
    }
}

void simulator_draw_controls(Simulator *sim, Body bodies[], float *sim_speed, bool *paused, int body_count, double sim_time_seconds) {
    *paused = widget_toggle(
        layout_anchor(105, 40, LAYOUT_TOP_LEFT, SIM_UI_LEFT_MARGIN, SIM_UI_PAUSE_Y),
        "Paused",
        *paused
    );

    if (widget_button(layout_anchor(105, 40, LAYOUT_TOP_LEFT, 135, SIM_UI_PAUSE_Y), "Reset")) {
        sim->requested_template_index = sim->active_template_index;
        sim->template_menu_open = false;
    }

    simulator_draw_time_display(sim_time_seconds);
    simulator_draw_options(sim);
    simulator_draw_templates_menu(sim);
    simulator_draw_advanced_menu(sim, bodies, body_count);
    simulator_draw_speed_control(sim, sim_speed);
    simulator_draw_navigation(sim);
    simulator_draw_body_lock_menu(sim, bodies, body_count);
    simulator_draw_save_prompt(sim);

    Rectangle speed_text = layout_anchor(220, 22, LAYOUT_TOP_LEFT, SIM_UI_LEFT_MARGIN, SIM_UI_SPEED_COUNTER_Y);
    DrawText(TextFormat("Speed: %.2fx", *sim_speed), (int)speed_text.x, (int)speed_text.y, 20, WHITE);
    DrawText(TextFormat("Bodies: %d", body_count), (int)SIM_UI_LEFT_MARGIN, (int)SIM_UI_BODY_COUNT_Y, 20, WHITE);
}
