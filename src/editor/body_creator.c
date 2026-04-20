#include "editor/body_creator.h"
#include "ui/widget.h"
#include "render/renderer.h"
#include "raylib.h"

void body_creator_init(BodyCreator *creator, int screen_width, int screen_height) {
    creator->open = false;

    creator->draft.mass = 10.0f;
    creator->draft.position = (Vector2){screen_width * 0.5f, screen_height * 0.5f};
    creator->draft.velocity = (Vector2){0.0f, 0.0f};
    creator->draft.force = (Vector2){0.0f, 0.0f};
}

bool body_creator_draw(BodyCreator *creator, int screen_width, int screen_height) {
    if (!creator->open) {
        return false;
    }

    Rectangle panel = { 500, 20, 260, 320 };

    DrawRectangleRec(panel, (Color){30, 30, 30, 230});
    DrawRectangleLinesEx(panel, 2.0f, WHITE);

    widget_label(520, 30, "Create Body", 24, WHITE);

    creator->draft.mass = widget_slider(
        (Rectangle){520, 80, 160, 20},
        1.0f, 1000.0f,
        creator->draft.mass,
        "Mass"
    );

    creator->draft.position.x = widget_slider(
        (Rectangle){520, 140, 160, 20},
        0.0f, (float)screen_width,
        creator->draft.position.x,
        "Position X"
    );

    creator->draft.position.y = widget_slider(
        (Rectangle){520, 200, 160, 20},
        0.0f, (float)screen_height,
        creator->draft.position.y,
        "Position Y"
    );

    creator->draft.velocity.x = widget_slider(
        (Rectangle){520, 260, 160, 20},
        -20.0f, 20.0f,
        creator->draft.velocity.x,
        "Velocity X"
    );

    creator->draft.velocity.y = widget_slider(
        (Rectangle){520, 320, 160, 20},
        -20.0f, 20.0f,
        creator->draft.velocity.y,
        "Velocity Y"
    );

    if (widget_button((Rectangle){520, 380, 100, 40}, "Create")) {
        creator->open = false;
        return true;
    }

    if (widget_button((Rectangle){640, 380, 100, 40}, "Cancel")) {
        creator->open = false;
    }

    return false;
}

void body_creator_draw_preview(const BodyCreator *creator) {
    if (!creator->open) {
        return;
    }

    render_body(creator->draft.position, 6.0f, GREEN);

    Vector2 end = {
        creator->draft.position.x + creator->draft.velocity.x * 10.0f,
        creator->draft.position.y + creator->draft.velocity.y * 10.0f
    };

    DrawLineV(creator->draft.position, end, GREEN);
}