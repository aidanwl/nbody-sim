#ifndef RENDERER_H
#define RENDERER_H

#include "core/simulation.h"

#include "core/vec2.h"

void render_body(Vector2 position, float radius, Color color);
void render_trail(const Body *body, float zoom, Vector2 camera_offset);
void render_velocity(const Body *body, float zoom, Vector2 camera_offset);

#endif
