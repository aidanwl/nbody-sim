#ifndef SIMULATOR_TEMPLATES_H
#define SIMULATOR_TEMPLATES_H

#include "core/body.h"

// Named starting system available from the Templates menu.
typedef struct {
    const char *name;
    const Body *bodies;
    int body_count;
} SimulatorTemplate;

// Template lookup/copy helpers used by app.c and simulator_ui.c.
int simulator_template_count(void);
const SimulatorTemplate *simulator_template_get(int index);
int simulator_template_copy_bodies(int index, Body bodies[], int max_bodies);

#endif
