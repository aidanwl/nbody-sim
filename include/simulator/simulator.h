#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdbool.h>
#include "raylib.h"
#include "core/body.h"

// Controls how much stored trail history is drawn.
typedef enum {
    PATH_MODE_OFF,
    PATH_MODE_SHORT,
    PATH_MODE_LONG,
    PATH_MODE_ORBIT
} PathMode;

// UI, camera, and draw state owned by the simulator layer.
typedef struct {
    float zoom;

    // Display options controlled by the Options panel.
    PathMode path_mode;
    bool show_current_trajectory;

    // Menu flags and action requests. app.c performs mutations requested here.
    bool speed_slider_open;
    bool body_menu_open;
    bool template_menu_open;
    bool advanced_menu_open;
    bool input_blocked;
    bool controls_blocked;
    bool save_system_requested;
    bool save_prompt_open;
    int requested_template_index;
    int requested_saved_system_index;
    int delete_saved_system_index;
    int active_template_index;
    int locked_body_index;
    int named_body_index;
    int stats_body_index;
    int delete_body_index;
    int edit_body_index;
    int saved_system_count;
    char saved_system_names[16][32];
    char save_system_name[32];

    // camera_focus is the world point at screen center; camera_pan is extra screen drag.
    Vector2 camera_focus;
    
    Vector2 camera_pan;

    Texture2D origin_icon;
} Simulator;

// Fixed world origin used by templates and the reset-to-origin button.
extern const Vector2 SIMULATOR_ORIGIN;

// Initializes, resets, updates, draws, and cleans up simulator state.
void simulator_init(Simulator *sim);
void simulator_deinit(Simulator *sim);
void simulator_reset(Simulator *sim);

void simulator_update(Simulator *sim, float frame_dt);

void simulator_draw(Simulator *sim, Body bodies[], int body_count, float *sim_speed, bool *paused, double sim_time_seconds);

void simulator_update_camera(Simulator *sim, float frame_dt);

void simulator_draw_controls(Simulator *sim, Body bodies[], float *sim_speed, bool *paused, int body_count, double sim_time_seconds);

// Coordinate conversion helpers between simulation/world space and screen pixels.
Vector2 simulator_screen_to_world(const Simulator *sim, Vector2 screen_pos);
Vector2 simulator_world_to_screen(const Simulator *sim, Vector2 world_pos);
void simulator_center_on_world(Simulator *sim, Vector2 world_pos);

#endif
