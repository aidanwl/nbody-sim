#ifndef APP_H
#define APP_H

#include <stdbool.h>
#include "core/body.h"
#include "editor/body_creator.h"

#define MAX_BODIES 128

// Top-level application state shared between the app loop, simulator UI, and body creator.
typedef struct {
    // Fixed-size body array keeps memory simple and avoids allocation during simulation.
    Body bodies[MAX_BODIES];
    int body_count;

    // dt is the real frame time; sim_speed scales how fast simulated time advances.
    float dt;
    float sim_speed;
    double sim_time_seconds;
    bool paused;

    // Stored so modal/editor UI can reset around the current window size.
    int screen_width;
    int screen_height;

    // Body creator state and request flags are processed by app_update.
    BodyCreator creator;
    BodyDraft pending_body;
    bool add_body_requested;
    bool edit_body_requested;
    int editing_body_index;
} App;

void app_init(App *app, int screen_width, int screen_height);
void app_update(App *app);
void app_draw(App *app);
void app_deinit(App *app);


#endif
