#include "simulator/simulator_templates.h"

#include <stddef.h>

// Built-in starting systems. Positions are centered around SIMULATOR_ORIGIN.
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

static const Body chaos_demo_bodies[] = {
    {
        .name = "Primary Star",
        .mass = 620.0f,
        .position = {350.0f, 300.0f},
        .velocity = {0.0f, -1.05f},
        .force = {0.0f, 0.0f},
        .color = {255, 203, 0, 255}
    },
    {
        .name = "Secondary Star",
        .mass = 620.0f,
        .position = {450.0f, 300.0f},
        .velocity = {0.0f, 1.05f},
        .force = {0.0f, 0.0f},
        .color = {102, 191, 255, 255}
    },
    {
        .name = "Probe A",
        .mass = 1.0f,
        .position = {400.0f, 185.0f},
        .velocity = {1.85f, 0.35f},
        .force = {0.0f, 0.0f},
        .color = {0, 228, 48, 255}
    },
    {
        .name = "Probe B",
        .mass = 1.0f,
        .position = {400.2f, 185.0f},
        .velocity = {1.85f, 0.35f},
        .force = {0.0f, 0.0f},
        .color = {255, 0, 128, 255}
    },
    {
        .name = "Perturber",
        .mass = 45.0f,
        .position = {400.0f, 450.0f},
        .velocity = {-1.55f, 0.0f},
        .force = {0.0f, 0.0f},
        .color = {255, 161, 0, 255}
    }
};

static const Body tatooine_system_bodies[] = {
    {
        .name = "Tatooine Sun A",
        .mass = 700.0f,
        .position = {370.0f, 300.0f},
        .velocity = {0.0f, -1.05f},
        .force = {0.0f, 0.0f},
        .color = {255, 210, 40, 255}
    },
    {
        .name = "Tatooine Sun B",
        .mass = 500.0f,
        .position = {430.0f, 300.0f},
        .velocity = {0.0f, 1.47f},
        .force = {0.0f, 0.0f},
        .color = {255, 120, 40, 255}
    },
    {
        .name = "Tatooine",
        .mass = 6.0f,
        .position = {400.0f, 125.0f},
        .velocity = {2.65f, 0.0f},
        .force = {0.0f, 0.0f},
        .color = {220, 180, 90, 255}
    },
    {
        .name = "Outer Desert Moon",
        .mass = 1.2f,
        .position = {400.0f, 80.0f},
        .velocity = {3.05f, 0.0f},
        .force = {0.0f, 0.0f},
        .color = {180, 150, 110, 255}
    },
    {
        .name = "Far Companion",
        .mass = 2.0f,
        .position = {400.0f, 520.0f},
        .velocity = {-2.05f, 0.0f},
        .force = {0.0f, 0.0f},
        .color = {120, 200, 255, 255}
    }
};

static const Body lagrange_point_bodies[] = {
    {
        .name = "Sun",
        .mass = 1000.0f,
        .position = {400.0f, 300.0f},
        .velocity = {0.0f, 0.0f},
        .force = {0.0f, 0.0f},
        .color = {255, 203, 0, 255}
    },
    {
        .name = "Planet",
        .mass = 12.0f,
        .position = {400.0f, 160.0f},
        .velocity = {2.65f, 0.0f},
        .force = {0.0f, 0.0f},
        .color = {0, 121, 241, 255}
    },

    {
        .name = "L4 Trojan",
        .mass = 0.05f,
        .position = {521.2f, 230.0f},
        .velocity = {1.325f, 2.295f},
        .force = {0.0f, 0.0f},
        .color = {0, 228, 48, 255}
    },
    {
        .name = "L5 Trojan",
        .mass = 0.05f,
        .position = {278.8f, 230.0f},
        .velocity = {1.325f, -2.295f},
        .force = {0.0f, 0.0f},
        .color = {255, 0, 128, 255}
    }
};

// Master template table used by the Templates menu.
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
    },
    {
        .name = "Chaos Theory Demo",
        .bodies = chaos_demo_bodies,
        .body_count = (int)(sizeof(chaos_demo_bodies) / sizeof(chaos_demo_bodies[0]))
    },
    {
        .name = "Tatooine System",
        .bodies = tatooine_system_bodies,
        .body_count = (int)(sizeof(tatooine_system_bodies) / sizeof(tatooine_system_bodies[0]))
    },
    {
        .name = "Lagrange Point Demo",
        .bodies = lagrange_point_bodies,
        .body_count = (int)(sizeof(lagrange_point_bodies) / sizeof(lagrange_point_bodies[0]))
    }
};

// Returns the number of built-in templates.
int simulator_template_count(void) {
    return (int)(sizeof(templates) / sizeof(templates[0]));
}

// Returns a template by index, or NULL for invalid indices.
const SimulatorTemplate *simulator_template_get(int index) {
    if (index < 0 || index >= simulator_template_count()) {
        return NULL;
    }

    return &templates[index];
}

// Copies template bodies into the app's live body array and clears runtime-only state.
int simulator_template_copy_bodies(int index, Body bodies[], int max_bodies) {
    const SimulatorTemplate *sim_template = simulator_template_get(index);

    if (sim_template == NULL || bodies == NULL || max_bodies <= 0) {
        return 0;
    }

    int body_count = sim_template->body_count;

    if (body_count > max_bodies) {
        body_count = max_bodies;
    }

    for (int i = 0; i < body_count; i++) {
        bodies[i] = sim_template->bodies[i];
        // Force and trail fields must start clean every time a template is loaded.
        bodies[i].force = (Vector2){0.0f, 0.0f};
        bodies[i].trail_count = 0;
        bodies[i].trail_start = 0;
        bodies[i].trail_sample_counter = 0;
    }

    return body_count;
}
