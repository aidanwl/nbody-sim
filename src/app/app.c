#include "app/app.h"

#include <stdio.h>

#include "raylib.h"

#include "core/simulation.h"
#include "editor/body_creator.h"
#include "simulator/simulator.h"
#include "simulator/simulator_templates.h"
#include "core/widget.h"

static Simulator simulator;
static float simulation_accumulator = 0.0f;

// Deletes body and shifts relevant body indicies down by 1
static void app_delete_body(App *app, int index) {
    if (index < 0 || index >= app->body_count) {
        return;
    }

    for (int i = index; i < app->body_count - 1; i++) {
        app->bodies[i] = app->bodies[i + 1];
    }

    app->body_count--;

    if (simulator.locked_body_index == index) {
        simulator.locked_body_index = -1;
    } else if (simulator.locked_body_index > index) {
        simulator.locked_body_index--;
    }

    if (simulator.named_body_index == index) {
        simulator.named_body_index = -1;
    } else if (simulator.named_body_index > index) {
        simulator.named_body_index--;
    }

    if (simulator.stats_body_index == index) {
        simulator.stats_body_index = -1;
    } else if (simulator.stats_body_index > index) {
        simulator.stats_body_index--;
    }
}

// Takes template and loads bodies into app
static void app_load_template(App *app, int template_index) {
    if (simulator_template_get(template_index) == NULL) {
        simulator.requested_template_index = -1;
        return;
    }

    app->body_count = simulator_template_copy_bodies(template_index, app->bodies, MAX_BODIES);
    app->sim_speed = 1.0f;
    app->paused = true;
    app->add_body_requested = false;
    simulation_accumulator = 0.0f;

    simulator_reset(&simulator);
    simulator.active_template_index = template_index;
    body_creator_init(&app->creator, app->screen_width, app->screen_height);
    body_creator_set_center_icon(&app->creator, simulator.origin_icon);
}

// Converts draft body into real one when clicking confirm
static void app_add_pending_body(App *app) {
    if (!app->add_body_requested) {
        return;
    }

    if (app->body_count < MAX_BODIES) {
        BodyDraft *draft = &app->pending_body;
        Body new_body = {
            .mass = draft->mass,
            .position = simulator_screen_to_world(&simulator, draft->position),
            .velocity = draft->velocity,
            .force = {0.0f, 0.0f},
            .color = draft->color,
            .trail_count = 0,
            .trail_start = 0,
            .trail_sample_counter = 0
        };

        snprintf(new_body.name, sizeof(new_body.name), "%s", draft->name);
        app->bodies[app->body_count] = new_body;
        app->body_count++;
    }

    app->add_body_requested = false;
}

// Centers camera on body (in body creator prompt)
static void app_handle_center_request(App *app) {
    if (!app->creator.center_requested) {
        return;
    }

    Vector2 world_pos = simulator_screen_to_world(&simulator, app->creator.draft.position);

    simulator_center_on_world(&simulator, world_pos);
    app->creator.draft.position = (Vector2){
        GetScreenWidth() * 0.5f,
        GetScreenHeight() * 0.5f
    };
    app->creator.center_requested = false;
}

// Sets the priority of different actions
static bool app_process_requests(App *app) {
    if (simulator.requested_template_index >= 0) {
        app_load_template(app, simulator.requested_template_index);
        return true;
    }

    if (simulator.delete_body_index >= 0) {
        app_delete_body(app, simulator.delete_body_index);
        simulator.delete_body_index = -1;
    }

    app_add_pending_body(app);
    app_handle_center_request(app);

    return false;
}

// Updates simulation by fixed time steps, independent of frame rate
static void app_update_simulation(App *app) {
    const float fixed_dt = 1.0f / 240.0f;

    if (app->paused) {
        return;
    }

    if (app->body_count <= 0) {
        return;
    }

    simulation_accumulator += app->dt * app->sim_speed;

    while (simulation_accumulator >= fixed_dt) {
        time_step(app->bodies, app->body_count, fixed_dt);
        simulation_accumulator -= fixed_dt;
    }
}

// Initializes app state and loads default template (blank)
void app_init(App *app, int screen_width, int screen_height) {
    app->screen_width = screen_width;
    app->screen_height = screen_height;

    app->dt = 0.0f;
    app->sim_speed = 1.0f;
    app->paused = true;
    app->add_body_requested = false;

    simulator_init(&simulator);
    app_load_template(app, simulator.active_template_index);
}

// Main update function for each frame
void app_update(App *app) {
    app->dt = GetFrameTime();

    if (app_process_requests(app)) {
        return;
    }

    simulator.input_blocked = body_creator_blocks_movement(&app->creator);
    if (!simulator.input_blocked && IsKeyPressed(KEY_SPACE)) {
        app->paused = !app->paused;
    }

    app_update_simulation(app);
    simulator_update(&simulator, app->dt);
}

// Main draw function for each frame
void app_draw(App *app) {

    simulator_draw(
        &simulator,
        app->bodies,
        app->body_count,
        &app->sim_speed,
        &app->paused
    );
    body_creator_draw_preview(&app->creator);

    if (widget_button((Rectangle){20, 220, 140, 40}, "New Body")) {
        body_creator_start(&app->creator, app->screen_width, app->screen_height);
    }

    if (body_creator_draw(&app->creator, app->screen_width, app->screen_height)) {
        app->pending_body = app->creator.draft;
        app->add_body_requested = true;
    }
}

// Clean up resources (just the origin button texture)
void app_deinit(App *app) {
    (void)app;
    simulator_deinit(&simulator);
}
