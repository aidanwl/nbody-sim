#include "app/app.h"

#include "raylib.h"

#include "core/simulation.h"
#include "editor/body_creator.h"
#include "simulator/simulator.h"
#include "core/widget.h"

// Temporary testing ground

static Simulator simulator;

static void app_init_bodies(App *app) {
    app->body_count = 4;

    app->bodies[0] = (Body){
        .mass = 600.0f,
        .position = {350.0f, 300.0f},
        .velocity = {0.0f, -1.15f},
        .force = {0.0f, 0.0f},
	.trail_count = 0,
	.trail_start = 0
    };

    app->bodies[1] = (Body){
        .mass = 600.0f,
        .position = {450.0f, 300.0f},
        .velocity = {0.0f, 1.15f},
        .force = {0.0f, 0.0f},
	.trail_count = 0,
    	.trail_start = 0
    };

    app->bodies[2] = (Body){
        .mass = 5.0f,
        .position = {400.0f, 210.0f},
        .velocity = {2.7f, 0.0f},
        .force = {0.0f, 0.0f},
	.trail_count = 0,
    	.trail_start = 0
    };

    app->bodies[3] = (Body){
        .mass = 2.0f,
        .position = {400.0f, 470.0f},
        .velocity = {-2.0f, 0.0f},
        .force = {0.0f, 0.0f},
	.trail_count = 0,
    	.trail_start = 0
    };
}

static void app_update_simulation(App *app) {
    static float accumulator = 0.0f;
    const float fixed_dt = 1.0f / 240.0f;

    if (app->paused) {
        return;
    }

    accumulator += app->dt * app->sim_speed;

    while (accumulator >= fixed_dt) {
        time_step(app->bodies, app->body_count, fixed_dt);
        accumulator -= fixed_dt;
    }
}

void app_init(App *app, int screen_width, int screen_height) {
    app->screen_width = screen_width;
    app->screen_height = screen_height;

    app->dt = 0.0f;
    app->sim_speed = 1.0f;
    app->paused = false;

    app_init_bodies(app);
    body_creator_init(&app->creator, screen_width, screen_height);

    simulator_init(&simulator);
}

void app_update(App *app) {
    app->dt = GetFrameTime();

    app_update_simulation(app);
    simulator_update(&simulator, app->dt);
}

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
        app->creator.open = true;
    }

    if (body_creator_draw(&app->creator, app->screen_width, app->screen_height)) {
        if (app->body_count < MAX_BODIES) {
            app->bodies[app->body_count] = app->creator.draft;
            app->body_count++;
        }
    }
}

void app_deinit(App *app) {
    simulator_deinit(&simulator);
}