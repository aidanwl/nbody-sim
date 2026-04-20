#ifndef BODY_CREATOR_H
#define BODY_CREATOR_H

#include <stdbool.h>
#include "core/body.h"

typedef struct {
    bool open;
    Body draft;
} BodyCreator;

void body_creator_init(BodyCreator *creator, int screen_width, int screen_height);
bool body_creator_draw(BodyCreator *creator, int screen_width, int screen_height);
void body_creator_draw_preview(const BodyCreator *creator);

#endif