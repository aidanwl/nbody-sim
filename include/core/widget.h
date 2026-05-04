#ifndef WIDGET_H
#define WIDGET_H

#include "raylib.h"

bool widget_button(Rectangle bounds, const char *text);
bool widget_toggle(Rectangle bounds, const char *text, bool value);
float widget_slider(Rectangle bounds, float min, float max, float value, const char *label);
bool widget_dropdown(Rectangle bounds, const char *text, bool open);
bool widget_image_button(Rectangle bounds, Texture2D tex);

#endif
