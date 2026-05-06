#ifndef BODY_CREATOR_H
#define BODY_CREATOR_H

#include <stdbool.h>
#include "core/body.h"

// Temporary body data used by the editor before it becomes a full Body with trail history.
typedef struct {
    char name[32];
    float mass;
    Vector2 position;
    Vector2 velocity;
    Color color;
} BodyDraft;

// State for the create/edit body panel and placement mode.
typedef struct {
    // open draws the panel; placing waits for a click in the world to choose position.
    bool open;
    bool placing;
    bool wait_for_release;
    bool center_requested;
    bool editing;
    int active_input;
    Texture2D center_icon;
    BodyDraft draft;
    char name_text[32];
    char mass_text[32];
    char velocity_x_text[32];
    char velocity_y_text[32];
} BodyCreator;

// Initializes editor state and creates the default draft values.
void body_creator_init(BodyCreator *creator, int screen_width, int screen_height);

// Stores the icon used by the center-on-draft button.
void body_creator_set_center_icon(BodyCreator *creator, Texture2D icon);

// Starts new-body placement mode.
void body_creator_start(BodyCreator *creator, int screen_width, int screen_height);

// Opens the editor with an existing body's values.
void body_creator_start_edit(BodyCreator *creator, BodyDraft draft);

// Draws the editor and returns true when Create/Save was confirmed.
bool body_creator_draw(BodyCreator *creator, int screen_width, int screen_height);

// Draws the pending body/velocity preview while placing or editing.
void body_creator_draw_preview(const BodyCreator *creator);

// Tells simulator input whether the editor should block camera/UI movement.
bool body_creator_blocks_movement(const BodyCreator *creator);

#endif
