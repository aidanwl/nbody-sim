#ifndef BODY_CREATOR_H
#define BODY_CREATOR_H

#include <stdbool.h>
#include "core/body.h"

typedef struct {
    char name[32];
    float mass;
    Vector2 position;
    Vector2 velocity;
    Color color;
} BodyDraft;

typedef struct {
    bool open;
    bool placing;
    bool wait_for_release;
    bool center_requested;
    int active_input;
    Texture2D center_icon;
    BodyDraft draft;
    char name_text[32];
    char mass_text[32];
    char velocity_x_text[32];
    char velocity_y_text[32];
} BodyCreator;

void body_creator_init(BodyCreator *creator, int screen_width, int screen_height);
void body_creator_set_center_icon(BodyCreator *creator, Texture2D icon);
void body_creator_start(BodyCreator *creator, int screen_width, int screen_height);
bool body_creator_draw(BodyCreator *creator, int screen_width, int screen_height);
void body_creator_draw_preview(const BodyCreator *creator);
bool body_creator_blocks_movement(const BodyCreator *creator);

#endif
