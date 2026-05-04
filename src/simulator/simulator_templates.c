#include "simulator/simulator_templates.h"

#include <stddef.h>

static const Body binary_system_bodies[] = {
    {
        .name = "Star A",
        .mass = 600.0f,
        .position = {350.0f, 300.0f},
        .velocity = {0.0f, -1.15f},
        .force = {0.0f, 0.0f},
        .color = {255, 203, 0, 255}
    },
    {
        .name = "Star B",
        .mass = 600.0f,
        .position = {450.0f, 300.0f},
        .velocity = {0.0f, 1.15f},
        .force = {0.0f, 0.0f},
        .color = {102, 191, 255, 255}
    },
    {
        .name = "Inner Planet",
        .mass = 5.0f,
        .position = {400.0f, 210.0f},
        .velocity = {2.7f, 0.0f},
        .force = {0.0f, 0.0f},
        .color = {0, 228, 48, 255}
    },
    {
        .name = "Outer Body",
        .mass = 2.0f,
        .position = {400.0f, 470.0f},
        .velocity = {-2.0f, 0.0f},
        .force = {0.0f, 0.0f},
        .color = {255, 161, 0, 255}
    }
};

static const SimulatorTemplate templates[] = {
    {
        .name = "Empty",
        .bodies = NULL,
        .body_count = 0
    },
    {
        .name = "Binary System",
        .bodies = binary_system_bodies,
        .body_count = (int)(sizeof(binary_system_bodies) / sizeof(binary_system_bodies[0]))
    }
};

int simulator_template_count(void) {
    return (int)(sizeof(templates) / sizeof(templates[0]));
}

const SimulatorTemplate *simulator_template_get(int index) {
    if (index < 0 || index >= simulator_template_count()) {
        return NULL;
    }

    return &templates[index];
}

int simulator_template_copy_bodies(int index, Body bodies[], int max_bodies) {
    const SimulatorTemplate *template = simulator_template_get(index);

    if (template == NULL || bodies == NULL || max_bodies <= 0) {
        return 0;
    }

    int body_count = template->body_count;

    if (body_count > max_bodies) {
        body_count = max_bodies;
    }

    for (int i = 0; i < body_count; i++) {
        bodies[i] = template->bodies[i];
        bodies[i].force = (Vector2){0.0f, 0.0f};
        bodies[i].trail_count = 0;
        bodies[i].trail_start = 0;
        bodies[i].trail_sample_counter = 0;
    }

    return body_count;
}
