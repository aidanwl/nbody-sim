#ifndef LAYOUT_H
#define LAYOUT_H

#include "raylib.h"

// Anchor positions used to place UI panels relative to the current window.
typedef enum {
    LAYOUT_TOP_LEFT,
    LAYOUT_TOP_RIGHT,
    LAYOUT_BOTTOM_LEFT,
    LAYOUT_BOTTOM_RIGHT,
    LAYOUT_CENTER
} LayoutAnchor;

// Builds a rectangle pinned to a screen edge/corner/center with margins.
Rectangle layout_anchor(float width, float height, LayoutAnchor anchor, float margin_x, float margin_y);
// Builds a child rectangle using percentages of a parent rectangle.
Rectangle layout_relative(Rectangle parent, float x, float y, float width, float height);

#endif
