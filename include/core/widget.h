#ifndef WIDGET_H
#define WIDGET_H

#include "raylib.h"
#include <stdio.h>

bool widget_button(Rectangle bounds, const char *text);
bool widget_toggle(Rectangle bounds, const char *text, bool value);
float widget_slider(Rectangle bounds, float min, float max, float value, const char *label);
void widget_label(int x, int y, const char *text, int font_size, Color color);

#endif