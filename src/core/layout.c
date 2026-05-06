#include "core/layout.h"

// Returns a screen-space rectangle anchored to one side/corner of the current window.
Rectangle layout_anchor(float width, float height, LayoutAnchor anchor, float margin_x, float margin_y) {
    // Pull the current size every frame so UI follows window resizing.
    float screen_width = (float)GetScreenWidth();
    float screen_height = (float)GetScreenHeight();

    Rectangle rect = {
        .x = 0.0f,
        .y = 0.0f,
        .width = width,
        .height = height
    };

    // Each case computes the top-left corner for the requested anchor.
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

// Converts normalized parent-relative coordinates into an absolute rectangle.
Rectangle layout_relative(Rectangle parent, float x, float y, float width, float height) {
    return (Rectangle){
        .x = parent.x + parent.width * x,
        .y = parent.y + parent.height * y,
        .width = parent.width * width,
        .height = parent.height * height
    };
}
