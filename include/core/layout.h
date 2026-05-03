#ifndef LAYOUT_H
#define LAYOUT_H

#include "raylib.h"

typedef enum {
    LAYOUT_TOP_LEFT,
    LAYOUT_TOP_RIGHT,
    LAYOUT_BOTTOM_LEFT,
    LAYOUT_BOTTOM_RIGHT,
    LAYOUT_CENTER
} LayoutAnchor;

Rectangle layout_anchor(float width, float height, LayoutAnchor anchor, float margin_x, float margin_y);
Rectangle layout_relative(Rectangle parent, float x, float y, float width, float height);

#endif