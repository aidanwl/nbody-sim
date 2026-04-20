#ifndef APP_H
#define APP_H

#include <stdbool.h>
#include "core/body.h"
#include "editor/body_creator.h"

#define MAX_BODIES 128

typedef struct {
    Body bodies[MAX_BODIES];
    int body_count;

    double dt;
    bool paused;

    int screen_width;
    int screen_height;

    BodyCreator creator;
} App;

void app_init(App *app, int screen_width, int screen_height);
void app_update(App *app);
void app_draw(const App *app);

#endif