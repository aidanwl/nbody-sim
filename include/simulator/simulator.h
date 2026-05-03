#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdbool.h>
#include "raylib.h"
#include "core/body.h"

typedef struct {
    Vector2 camera_offset;
    float zoom;

    bool show_paths;
    bool show_current_trajectory;

    bool speed_slider_open;

    Vector2 camera_focus;
    
    Vector2 camera_pan;
    bool camera_user_moved;

    Texture2D origin_icon;
} Simulator;

void simulator_init(Simulator *sim);

void simulator_update(Simulator *sim, float frame_dt);

void simulator_draw(Simulator *sim, Body bodies[], int body_count, float *sim_speed, bool *paused);

void simulator_update_camera(Simulator *sim, float frame_dt);

void simulator_draw_controls(Simulator *sim, float *sim_speed, bool *paused, int body_count);

#endif