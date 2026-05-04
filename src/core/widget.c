#include "core/widget.h"
#include <stdio.h>

// ----------------- Button ----------------------------

bool widget_button(Rectangle bounds, const char *text) {
    Vector2 mouse = GetMousePosition();

    bool hovered = CheckCollisionPointRec(mouse, bounds);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    Color color = WHITE;
    if (hovered) {
        color = GRAY;
    }

    DrawRectangleRec(bounds, color);
    DrawRectangleLinesEx(bounds, 2.0f, BLACK);

    int font_size = 20;
    int text_width = MeasureText(text, font_size);

    int text_x = (int)(bounds.x + (bounds.width - text_width) * 0.5f);
    int text_y = (int)(bounds.y + (bounds.height - font_size) * 0.5f);

    DrawText(text, text_x, text_y, font_size, BLACK);

    return clicked;

}

// ----------------- Toggle ----------------------------

bool widget_toggle(Rectangle bounds, const char *text, bool value) {
    Vector2 mouse = GetMousePosition();

    bool hovered = CheckCollisionPointRec(mouse, bounds);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    if (clicked) {
        value = !value;
    }

    Color fill;
    if (value) {
        fill = hovered ? DARKGREEN : GREEN;
    } else {
        fill = hovered ? GRAY : LIGHTGRAY;
    }

    DrawRectangleRec(bounds, fill);
    DrawRectangleLinesEx(bounds, 2.0f, BLACK);

    int font_size = 20;
    int text_width = MeasureText(text, font_size);
    int text_x = (int)(bounds.x + (bounds.width - text_width) * 0.5f);
    int text_y = (int)(bounds.y + (bounds.height - font_size) * 0.5f);

    DrawText(text, text_x, text_y, font_size, BLACK);

    return value;
}

// ----------------- Slider ----------------------------

static float clampf(float x, float min, float max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

float widget_slider(Rectangle bounds, float min, float max, float value, const char *label) {
    static bool has_active_slider = false;
    static Rectangle active_slider = {0};
    Vector2 mouse = GetMousePosition();

    Rectangle track = {
        bounds.x,
        bounds.y + bounds.height * 0.5f - 3.0f,
        bounds.width,
        6.0f
    };

    float t = (value - min) / (max - min);
    t = clampf(t, 0.0f, 1.0f);

    float knob_x = bounds.x + t * bounds.width;
    Rectangle knob = {
        knob_x - 6.0f,
        bounds.y,
        12.0f,
        bounds.height
    };

    bool hovered = CheckCollisionPointRec(mouse, bounds);

    if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        has_active_slider = true;
        active_slider = bounds;
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        has_active_slider = false;
    }

    bool dragging = has_active_slider &&
                    active_slider.x == bounds.x &&
                    active_slider.y == bounds.y &&
                    active_slider.width == bounds.width &&
                    active_slider.height == bounds.height &&
                    IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    if (dragging) {
        float new_t = (mouse.x - bounds.x) / bounds.width;
        new_t = clampf(new_t, 0.0f, 1.0f);
        value = min + new_t * (max - min);

        t = new_t;
        knob_x = bounds.x + t * bounds.width;
        knob.x = knob_x - 6.0f;
    }

    DrawText(label, (int)bounds.x, (int)(bounds.y - 24), 20, WHITE);
    DrawRectangleRec(track, GRAY);
    DrawRectangleRec(knob, LIGHTGRAY);
    DrawRectangleLinesEx(knob, 1.0f, BLACK);

    char value_text[64];
    snprintf(value_text, sizeof(value_text), "%.3f", value);
    DrawText(value_text, (int)(bounds.x + bounds.width + 12), (int)(bounds.y + 2), 20, WHITE);

    return value;
}

bool widget_dropdown_header(Rectangle bounds, const char *text, bool open) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, bounds);

    if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        open = !open;
    }

    DrawRectangleRec(bounds, hovered ? GRAY : LIGHTGRAY);
    DrawRectangleLinesEx(bounds, 2.0f, BLACK);
    DrawText(open ? "v" : ">", (int)(bounds.x + 8), (int)(bounds.y + 5), 20, BLACK);
    DrawText(text, (int)(bounds.x + 30), (int)(bounds.y + 5), 20, BLACK);

    return open;
}

// ----------------- Origin Button ----------------------------
bool widget_image_button(Rectangle bounds, Texture2D tex) {
    Vector2 mouse = GetMousePosition();

    bool hovered = CheckCollisionPointRec(mouse, bounds);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    Color tint = hovered ? (Color){200, 200, 200, 255} : WHITE;

    bool pressed = hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    if (pressed) {
        bounds.x += 1;
        bounds.y += 1;
    }

    DrawTexturePro(
        tex,
        (Rectangle){0, 0, (float)tex.width, (float)tex.height},
        bounds,
        (Vector2){0, 0},
        0.0f,
        tint
    );

    return clicked;
}
