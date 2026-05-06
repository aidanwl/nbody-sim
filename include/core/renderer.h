#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "core/vec2.h"

// Draws one body at its already-converted screen position.
void render_body(Vector2 position, float radius, Color color);

#endif
