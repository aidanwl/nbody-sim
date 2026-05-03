#include "editor/body_creator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/renderer.h"
#include "core/widget.h"
#include "raylib.h"

enum {
    BODY_CREATOR_INPUT_NONE = 0,
    BODY_CREATOR_INPUT_NAME,
    BODY_CREATOR_INPUT_MASS,
    BODY_CREATOR_INPUT_VELOCITY_X,
    BODY_CREATOR_INPUT_VELOCITY_Y
};

typedef struct {
    Rectangle panel;
    Rectangle name_input;
    Rectangle mass_input;
    Rectangle velocity_x_input;
    Rectangle velocity_y_input;
} BodyCreatorLayout;

static float clampf(float x, float min, float max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

static bool text_has_char(const char *text, char target) {
    return strchr(text, target) != NULL;
}

static bool numeric_key_allowed(char key, const char *buffer, int len) {
    if (key >= '0' && key <= '9') {
        return true;
    }

    if (key == '-' && len == 0) {
        return true;
    }

    if (key == '.' && !text_has_char(buffer, '.')) {
        return true;
    }

    return false;
}

static void body_creator_sync_text(BodyCreator *creator) {
    snprintf(creator->name_text, sizeof(creator->name_text), "%s", creator->draft.name);
    snprintf(creator->mass_text, sizeof(creator->mass_text), "%.2f", creator->draft.mass);
    snprintf(creator->velocity_x_text, sizeof(creator->velocity_x_text), "%.2f", creator->draft.velocity.x);
    snprintf(creator->velocity_y_text, sizeof(creator->velocity_y_text), "%.2f", creator->draft.velocity.y);
}

static void body_creator_reset_draft(BodyCreator *creator, int screen_width, int screen_height) {
    snprintf(creator->draft.name, sizeof(creator->draft.name), "New Body");
    creator->draft.mass = 10.0f;
    creator->draft.position = (Vector2){screen_width * 0.5f, screen_height * 0.5f};
    creator->draft.velocity = (Vector2){0.0f, 0.0f};
    creator->draft.color = GREEN;
    creator->active_input = BODY_CREATOR_INPUT_NONE;
    body_creator_sync_text(creator);
}

static void draw_text_input(Rectangle bounds, const char *label, char *buffer, int buffer_size, int input_id, bool numeric_only, BodyCreator *creator) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, bounds);
    bool active = creator->active_input == input_id;

    if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        creator->active_input = input_id;
    }

    DrawText(label, (int)bounds.x, (int)(bounds.y - 22), 18, WHITE);
    DrawRectangleRec(bounds, active ? RAYWHITE : LIGHTGRAY);
    DrawRectangleLinesEx(bounds, 2.0f, active ? SKYBLUE : BLACK);
    DrawText(buffer, (int)(bounds.x + 8), (int)(bounds.y + 8), 20, BLACK);

    if (!active) {
        return;
    }

    int len = (int)strlen(buffer);
    int key = GetCharPressed();

    while (key > 0) {
        bool allowed = key >= 32 && key <= 126;

        if (numeric_only) {
            allowed = numeric_key_allowed((char)key, buffer, len);
        }

        if (allowed && len < buffer_size - 1) {
            buffer[len++] = (char)key;
            buffer[len] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && len > 0) {
        buffer[len - 1] = '\0';
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_TAB)) {
        creator->active_input = BODY_CREATOR_INPUT_NONE;
    }
}

static float parse_float_input(const char *text, float fallback, float min, float max) {
    char *end = NULL;
    float value = strtof(text, &end);

    if (end == text) {
        return fallback;
    }

    return clampf(value, min, max);
}

static Color color_with_alpha(Color color, unsigned char alpha) {
    color.a = alpha;
    return color;
}

static bool rect_contains_mouse(Rectangle bounds) {
    return CheckCollisionPointRec(GetMousePosition(), bounds);
}

static void draw_color_swatch(BodyCreator *creator, Rectangle bounds, Color color) {
    bool selected = creator->draft.color.r == color.r &&
                    creator->draft.color.g == color.g &&
                    creator->draft.color.b == color.b;

    DrawRectangleRec(bounds, color);
    DrawRectangleLinesEx(bounds, selected ? 4.0f : 2.0f, selected ? WHITE : BLACK);

    if (CheckCollisionPointRec(GetMousePosition(), bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        creator->draft.color = color;
    }
}

static Rectangle body_creator_panel_bounds(int screen_width, int screen_height) {
    float margin = 20.0f;
    float top = 270.0f;
    float width = screen_width * 0.42f;
    float available_height = screen_height - top - margin;
    float height = available_height;

    if (width < 360.0f) {
        width = 360.0f;
    }
    if (width > 460.0f) {
        width = 460.0f;
    }

    if (height > 480.0f) {
        height = 480.0f;
    }
    if (height < 420.0f && available_height >= 420.0f) {
        height = 420.0f;
    }
    if (height < 320.0f) {
        height = available_height;
    }

    return (Rectangle){margin, top, width, height};
}

static Rectangle panel_rect(Rectangle panel, float x, float y, float width, float height) {
    return (Rectangle){
        panel.x + panel.width * x,
        panel.y + panel.height * y,
        panel.width * width,
        panel.height * height
    };
}

static BodyCreatorLayout body_creator_layout(int screen_width, int screen_height) {
    BodyCreatorLayout layout = {0};

    layout.panel = body_creator_panel_bounds(screen_width, screen_height);
    layout.name_input = panel_rect(layout.panel, 0.06f, 0.14f, 0.58f, 0.075f);
    layout.mass_input = panel_rect(layout.panel, 0.06f, 0.30f, 0.30f, 0.075f);
    layout.velocity_x_input = panel_rect(layout.panel, 0.06f, 0.48f, 0.30f, 0.075f);
    layout.velocity_y_input = panel_rect(layout.panel, 0.06f, 0.66f, 0.30f, 0.075f);

    return layout;
}

static bool body_creator_handle_placement(BodyCreator *creator) {
    if (!creator->placing) {
        return false;
    }

    if (creator->wait_for_release) {
        if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            creator->wait_for_release = false;
        }
        return true;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        creator->draft.position = GetMousePosition();
        creator->placing = false;
        creator->open = true;
    }

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) || IsKeyPressed(KEY_ESCAPE)) {
        creator->placing = false;
    }

    return true;
}

static void body_creator_clear_input_on_outside_click(BodyCreator *creator, Rectangle inputs[], int input_count) {
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return;
    }

    for (int i = 0; i < input_count; i++) {
        if (rect_contains_mouse(inputs[i])) {
            return;
        }
    }

    creator->active_input = BODY_CREATOR_INPUT_NONE;
}

static void body_creator_draw_header(BodyCreator *creator, Rectangle panel, float label_x) {
    DrawRectangleRec(panel, (Color){30, 30, 30, 235});
    DrawRectangleLinesEx(panel, 2.0f, WHITE);
    DrawText("Create Body", (int)label_x, (int)(panel.y + 12), 24, WHITE);

    Rectangle center_button = {
        label_x + (float)MeasureText("Create Body", 24) + 14.0f,
        panel.y + 10.0f,
        30.0f,
        30.0f
    };

    if (creator->center_icon.id != 0 && widget_image_button(center_button, creator->center_icon)) {
        creator->center_requested = true;
    }
}

static void body_creator_draw_fields(BodyCreator *creator, BodyCreatorLayout layout) {
    draw_text_input(layout.name_input, "Name", creator->name_text, sizeof(creator->name_text), BODY_CREATOR_INPUT_NAME, false, creator);
    snprintf(creator->draft.name, sizeof(creator->draft.name), "%s", creator->name_text[0] == '\0' ? "Unnamed" : creator->name_text);

    draw_text_input(layout.mass_input, "Mass", creator->mass_text, sizeof(creator->mass_text), BODY_CREATOR_INPUT_MASS, true, creator);
    creator->draft.mass = parse_float_input(creator->mass_text, creator->draft.mass, 1.0f, 1000.0f);
    creator->draft.mass = widget_slider(panel_rect(layout.panel, 0.43f, 0.335f, 0.38f, 0.045f), 1.0f, 1000.0f, creator->draft.mass, "Mass");
    if (creator->active_input != BODY_CREATOR_INPUT_MASS && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        body_creator_sync_text(creator);
    }

    draw_text_input(layout.velocity_x_input, "Velocity X", creator->velocity_x_text, sizeof(creator->velocity_x_text), BODY_CREATOR_INPUT_VELOCITY_X, true, creator);
    creator->draft.velocity.x = parse_float_input(creator->velocity_x_text, creator->draft.velocity.x, -20.0f, 20.0f);
    creator->draft.velocity.x = widget_slider(panel_rect(layout.panel, 0.43f, 0.515f, 0.38f, 0.045f), -20.0f, 20.0f, creator->draft.velocity.x, "Velocity X");

    draw_text_input(layout.velocity_y_input, "Velocity Y", creator->velocity_y_text, sizeof(creator->velocity_y_text), BODY_CREATOR_INPUT_VELOCITY_Y, true, creator);
    creator->draft.velocity.y = parse_float_input(creator->velocity_y_text, creator->draft.velocity.y, -20.0f, 20.0f);
    creator->draft.velocity.y = widget_slider(panel_rect(layout.panel, 0.43f, 0.695f, 0.38f, 0.045f), -20.0f, 20.0f, creator->draft.velocity.y, "Velocity Y");

    if (creator->active_input == BODY_CREATOR_INPUT_NONE && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        body_creator_sync_text(creator);
    }
}

static void body_creator_draw_color_picker(BodyCreator *creator, Rectangle panel, float label_x) {
    DrawText("Color", (int)label_x, (int)(panel.y + panel.height * 0.82f), 18, WHITE);
    draw_color_swatch(creator, panel_rect(panel, 0.06f, 0.88f, 0.08f, 0.07f), GREEN);
    draw_color_swatch(creator, panel_rect(panel, 0.17f, 0.88f, 0.08f, 0.07f), SKYBLUE);
    draw_color_swatch(creator, panel_rect(panel, 0.28f, 0.88f, 0.08f, 0.07f), ORANGE);
    draw_color_swatch(creator, panel_rect(panel, 0.39f, 0.88f, 0.08f, 0.07f), PINK);
    draw_color_swatch(creator, panel_rect(panel, 0.50f, 0.88f, 0.08f, 0.07f), VIOLET);
}

static bool body_creator_draw_actions(BodyCreator *creator, Rectangle panel) {
    if (widget_button(panel_rect(panel, 0.68f, 0.12f, 0.25f, 0.085f), "Create")) {
        creator->open = false;
        creator->placing = false;
        creator->active_input = BODY_CREATOR_INPUT_NONE;
        return true;
    }

    if (widget_button(panel_rect(panel, 0.68f, 0.23f, 0.25f, 0.085f), "Cancel")) {
        creator->open = false;
        creator->placing = false;
        creator->active_input = BODY_CREATOR_INPUT_NONE;
    }

    return false;
}

void body_creator_init(BodyCreator *creator, int screen_width, int screen_height) {
    creator->open = false;
    creator->placing = false;
    creator->wait_for_release = false;
    creator->center_requested = false;
    creator->center_icon = (Texture2D){0};
    body_creator_reset_draft(creator, screen_width, screen_height);
}

void body_creator_set_center_icon(BodyCreator *creator, Texture2D icon) {
    creator->center_icon = icon;
}

void body_creator_start(BodyCreator *creator, int screen_width, int screen_height) {
    body_creator_reset_draft(creator, screen_width, screen_height);
    creator->open = false;
    creator->placing = true;
    creator->wait_for_release = true;
    creator->center_requested = false;
    creator->active_input = BODY_CREATOR_INPUT_NONE;
}

bool body_creator_draw(BodyCreator *creator, int screen_width, int screen_height) {
    if (body_creator_handle_placement(creator)) {
        return false;
    }

    if (!creator->open) {
        return false;
    }

    BodyCreatorLayout layout = body_creator_layout(screen_width, screen_height);
    Rectangle panel = layout.panel;
    float label_x = panel.x + 20.0f;
    Rectangle inputs[] = {layout.name_input, layout.mass_input, layout.velocity_x_input, layout.velocity_y_input};

    body_creator_clear_input_on_outside_click(creator, inputs, 4);
    body_creator_draw_header(creator, panel, label_x);
    body_creator_draw_fields(creator, layout);
    body_creator_draw_color_picker(creator, panel, label_x);
    return body_creator_draw_actions(creator, panel);
}

void body_creator_draw_preview(const BodyCreator *creator) {
    if (creator->placing) {
        DrawCircleV(GetMousePosition(), 6.0f, color_with_alpha(GREEN, 130));
        return;
    }

    if (!creator->open) {
        return;
    }

    render_body(creator->draft.position, 6.0f, creator->draft.color);

    Vector2 end = {
        creator->draft.position.x + creator->draft.velocity.x * 10.0f,
        creator->draft.position.y + creator->draft.velocity.y * 10.0f
    };

    DrawLineV(creator->draft.position, end, creator->draft.color);
}

bool body_creator_blocks_movement(const BodyCreator *creator) {
    return creator->active_input != BODY_CREATOR_INPUT_NONE;
}
