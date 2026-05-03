#include "core/layout.h"

Rectangle layout_anchor(float width, float height, LayoutAnchor anchor, float margin_x, float margin_y) {
    float screen_width = (float)GetScreenWidth();
    float screen_height = (float)GetScreenHeight();

    Rectangle rect = {
        .x = 0.0f,
        .y = 0.0f,
        .width = width,
        .height = height
    };

    switch (anchor) {
        case LAYOUT_TOP_LEFT:
            rect.x = margin_x;
            rect.y = margin_y;
            break;

        case LAYOUT_TOP_RIGHT:
            rect.x = screen_width - width - margin_x;
            rect.y = margin_y;
            break;

        case LAYOUT_BOTTOM_LEFT:
            rect.x = margin_x;
            rect.y = screen_height - height - margin_y;
            break;

        case LAYOUT_BOTTOM_RIGHT:
            rect.x = screen_width - width - margin_x;
            rect.y = screen_height - height - margin_y;
            break;

        case LAYOUT_CENTER:
            rect.x = (screen_width - width) * 0.5f;
            rect.y = (screen_height - height) * 0.5f;
            break;
    }

    return rect;
}

Rectangle layout_relative(Rectangle parent, float x, float y, float width, float height) {
    return (Rectangle){
        .x = parent.x + parent.width * x,
        .y = parent.y + parent.height * y,
        .width = parent.width * width,
        .height = parent.height * height
    };
}

Rectangle layout_rect(float x, float y, float w, float h) {
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();

    return (Rectangle){
        x * sw,
        y * sh,
        w * sw,
        h * sh
    };
}