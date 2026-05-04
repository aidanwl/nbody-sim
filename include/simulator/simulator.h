#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdbool.h>
#include "raylib.h"
#include "core/body.h"

typedef enum {
    PATH_MODE_OFF,
    PATH_MODE_SHORT,
    PATH_MODE_LONG,
    PATH_MODE_ORBIT
} PathMode;

typedef struct {
    float zoom;

    PathMode path_mode;
    bool show_current_trajectory;

    bool speed_slider_open;
    bool body_menu_open;
    bool template_menu_open;
    bool input_blocked;
    int requested_template_index;
    int active_template_index;
    int locked_body_index;
    int named_body_index;
    int delete_body_index;

    Vector2 camera_focus;
    
    Vector2 camera_pan;

    Texture2D origin_icon;
} Simulator;

void simulator_init(Simulator *sim);
void simulator_deinit(Simulator *sim);
void simulator_reset(Simulator *sim);

void simulator_update(Simulator *sim, float frame_dt);

void simulator_draw(Simulator *sim, Body bodies[], int body_count, float *sim_speed, bool *paused);

void simulator_update_camera(Simulator *sim, float frame_dt);

void simulator_draw_controls(Simulator *sim, Body bodies[], float *sim_speed, bool *paused, int body_count);
Vector2 simulator_screen_to_world(const Simulator *sim, Vector2 screen_pos);
void simulator_center_on_world(Simulator *sim, Vector2 world_pos);

#endif
