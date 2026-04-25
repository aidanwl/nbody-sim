#include "app/app.h"

#include "raylib.h"

#include "core/simulation.h"
#include "editor/body_creator.h"
#include "simulator/simulator.h"

// Temporary testing ground

static Simulator simulator;

static void app_init_bodies(App *app) {
    app->body_count = 4;

    app->bodies[0] = (Body){
        .mass = 600.0f,
        .position = {350.0f, 300.0f},
        .velocity = {0.0f, -1.15f},
        .force = {0.0f, 0.0f}
    };

    app->bodies[1] = (Body){
        .mass = 600.0f,
        .position = {450.0f, 300.0f},
        .velocity = {0.0f, 1.15f},
        .force = {0.0f, 0.0f}
    };

    app->bodies[2] = (Body){
        .mass = 5.0f,
        .position = {400.0f, 210.0f},
        .velocity = {2.7f, 0.0f},
        .force = {0.0f, 0.0f}
    };

    app->bodies[3] = (Body){
        .mass = 2.0f,
        .position = {400.0f, 470.0f},
        .velocity = {-2.0f, 0.0f},
        .force = {0.0f, 0.0f}
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
}

/*
static void app_init_bodies(App *app) {
    app->body_count = 3;

    // Central body
    app->bodies[0].mass = 1000.0f;
    app->bodies[0].position = (Vector2){400.0f, 300.0f};
    app->bodies[0].velocity = (Vector2){0.0f, 0.0f};
    app->bodies[0].force = (Vector2){0.0f, 0.0f};

    // Orbiting body
    app->bodies[1].mass = 10.0f;
    app->bodies[1].position = (Vector2){414.0f, 300.0f};
    app->bodies[1].velocity = (Vector2){0.0f, sqrt(G * app->bodies[0].mass / 10.0f)};
    app->bodies[1].force = (Vector2){0.0f, 0.0f};

    // Orbiting body 2
    app->bodies[2].mass = 1.0f;
    app->bodies[2].position = (Vector2){490.0f, 300.0f};
    app->bodies[2].velocity = (Vector2){0.0f, sqrt(G * app->bodies[1].mass / 10.0f)};
    app->bodies[2].force = (Vector2){0.0f, 0.0f};
}

*/


/*
static void app_init_bodies(App *app) {
    app->body_count = 4;

    // Star A
    app->bodies[0].mass = 600.0f;
    app->bodies[0].position = (Vector2){350.0f, 300.0f};
    app->bodies[0].velocity = (Vector2){0.0f, -1.15f};
    app->bodies[0].force = (Vector2){0.0f, 0.0f};

    // Star B
    app->bodies[1].mass = 600.0f;
    app->bodies[1].position = (Vector2){450.0f, 300.0f};
    app->bodies[1].velocity = (Vector2){0.0f, 1.15f};
    app->bodies[1].force = (Vector2){0.0f, 0.0f};

    // Inner planet
    app->bodies[2].mass = 5.0f;
    app->bodies[2].position = (Vector2){400.0f, 210.0f};
    app->bodies[2].velocity = (Vector2){2.7f, 0.0f};
    app->bodies[2].force = (Vector2){0.0f, 0.0f};

    // Outer planet / comet
    app->bodies[3].mass = 2.0f;
    app->bodies[3].position = (Vector2){400.0f, 470.0f};
    app->bodies[3].velocity = (Vector2){-2.0f, 0.0f};
    app->bodies[3].force = (Vector2){0.0f, 0.0f};
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

static void app_draw_controls(App *app) {
    app->paused = widget_toggle((Rectangle){20, 20, 140, 40}, "Paused", app->paused);

    app->sim_speed = widget_slider(
        (Rectangle){20, 100, 200, 20},
        0.0f,
        20.0f,
        app->sim_speed,
        "Sim Speed"
    );

    if (widget_button((Rectangle){20, 220, 140, 40}, "New Body")) {
        app->creator.open = true;
    }

    DrawText(TextFormat("Current Time Step: %.2fx", app->sim_speed), 40, 190, 20, WHITE);
    DrawText(TextFormat("Bodies: %d", app->body_count), 40, 280, 20, WHITE);
}

static void app_draw_bodies(const App *app) {
    for (int i = 0; i < app->body_count; i++) {
        Color color = WHITE;

        if (i == 1) {
            color = RED;
        } else if (i == 2) {
            color = BLUE;
        }

        render_body(app->bodies[i].position, 5.0f, color);
    }
}

void app_init(App *app, int screen_width, int screen_height) {
    app->screen_width = screen_width;
    app->screen_height = screen_height;
    app->dt = 0.0f;
    app->paused = false;
    app->sim_speed = 5.0f;

    app_init_bodies(app);
    body_creator_init(&app->creator, screen_width, screen_height);
}

void app_update(App *app) {
    app->dt = GetFrameTime();
    app_update_simulation(app);
}

void app_draw(const App *app_const) {
    App *app = (App *)app_const;

    app_draw_controls(app);
    app_draw_bodies(app);

    body_creator_draw_preview(&app->creator);

    if (body_creator_draw(&app->creator, app->screen_width, app->screen_height)) {
        if (app->body_count < MAX_BODIES) {
            app->bodies[app->body_count] = app->creator.draft;
            app->body_count++;
        }
    }
}
*/
