#include "app/app.h"

#include <stdio.h>
#include <string.h>

#include "raylib.h"

#include "core/simulation.h"
#include "core/layout.h"
#include "editor/body_creator.h"
#include "simulator/simulator.h"
#include "simulator/simulator_templates.h"
#include "core/widget.h"

static Simulator simulator;
// Accumulates scaled frame time so physics can run at a stable fixed timestep.
static float simulation_accumulator = 0.0f;

#define USER_SYSTEM_FILE "user_systems.txt"
#define MAX_SAVED_SYSTEMS 16

// Trims newline characters from end of text (for reading system names from file)
static void trim_newline(char *text) {
    text[strcspn(text, "\r\n")] = '\0';
}

// Checks if text has any visible characters (prevents saving systems with blank names)
static bool text_has_visible_char(const char *text) {
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] != ' ' && text[i] != '\t') {
            return true;
        }
    }

    return false;
}

// Loads saved system names from text file in build folder into templates menu
static void app_refresh_saved_systems(void) {
    FILE *file = fopen(USER_SYSTEM_FILE, "r");
    char line[256];

    // Rebuild the list from scratch so the Templates menu matches the file.
    simulator.saved_system_count = 0;

    if (file == NULL) {
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL && simulator.saved_system_count < MAX_SAVED_SYSTEMS) {
        // Saved systems start with "S "; body lines start with "B ".
        if (line[0] != 'S' || line[1] != ' ') {
            continue;
        }

        trim_newline(line);
        snprintf(
            simulator.saved_system_names[simulator.saved_system_count],
            sizeof(simulator.saved_system_names[simulator.saved_system_count]),
            "%s",
            line + 2
        );
        simulator.saved_system_count++;
    }

    fclose(file);
}

// Resets body trail data when loading from template or saved system, or when editing body
static void app_reset_body_trail(Body *body) {
    body->force = (Vector2){0.0f, 0.0f};
    body->trail_count = 0;
    body->trail_start = 0;
    body->trail_sample_counter = 0;
}

// Converts body draft (used in body creator) into real body when adding or editing
static Body app_body_from_draft(const BodyDraft *draft) {
    Body body = {
        .mass = draft->mass,
        // Draft position is stored in screen space while editing, so convert before simulating.
        .position = simulator_screen_to_world(&simulator, draft->position),
        .velocity = draft->velocity,
        .force = {0.0f, 0.0f},
        .color = draft->color,
        .trail_count = 0,
        .trail_start = 0,
        .trail_sample_counter = 0
    };

    snprintf(body.name, sizeof(body.name), "%s", draft->name);
    return body;
}

// Saves system into text file in build folder (creating if it doesn't exist, and appending if it does), then refreshes saved systems in templates menu
static void app_save_system(App *app) {
    FILE *file = fopen(USER_SYSTEM_FILE, "a");
    char system_name[sizeof(simulator.save_system_name)];

    // Clear the request immediately so a failed save does not repeat every frame.
    simulator.save_system_requested = false;
    simulator.save_prompt_open = false;

    if (file == NULL) {
        return;
    }

    snprintf(system_name, sizeof(system_name), "%s", simulator.save_system_name);
    trim_newline(system_name);

    if (!text_has_visible_char(system_name)) {
        snprintf(system_name, sizeof(system_name), "Saved System %d", simulator.saved_system_count + 1);
    }

    // File format: S line names the system, N line stores body count, B lines store bodies.
    fprintf(file, "S %s\n", system_name);
    fprintf(file, "N %d\n", app->body_count);

    for (int i = 0; i < app->body_count; i++) {
        Body *body = &app->bodies[i];

        fprintf(
            file,
            "B %.9g %.9g %.9g %.9g %.9g %u %u %u %u %s\n",
            body->mass,
            body->position.x,
            body->position.y,
            body->velocity.x,
            body->velocity.y,
            (unsigned int)body->color.r,
            (unsigned int)body->color.g,
            (unsigned int)body->color.b,
            (unsigned int)body->color.a,
            body->name
        );
    }

    fclose(file);
    simulator.save_system_name[0] = '\0';
    app_refresh_saved_systems();
}

// Parses body data from saved system file, return true if successful, false if line is malformed
static bool parse_saved_body(const char *line, Body *body) {
    unsigned int r = 255;
    unsigned int g = 255;
    unsigned int b = 255;
    unsigned int a = 255;
    int name_offset = 0;

    // %n records where the body name begins after all numeric/color fields.
    int matched = sscanf(
        line,
        "B %f %f %f %f %f %u %u %u %u %n",
        &body->mass,
        &body->position.x,
        &body->position.y,
        &body->velocity.x,
        &body->velocity.y,
        &r,
        &g,
        &b,
        &a,
        &name_offset
    );

    if (matched != 9 || name_offset <= 0) {
        return false;
    }

    // The name may contain spaces, so copy the rest of the line instead of using %s.
    snprintf(body->name, sizeof(body->name), "%s", line + name_offset);
    trim_newline(body->name);
    body->color = (Color){(unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a};
    app_reset_body_trail(body);
    return true;
}

// Loads bodies from a system file, resets simulation state, and preserves path/trajectory settings; returns false if the file is invalid.
static bool app_load_saved_system(App *app, int saved_index) {
    FILE *file = fopen(USER_SYSTEM_FILE, "r");
    char line[256];
    int current_index = -1;

    simulator.requested_saved_system_index = -1;

    if (file == NULL || saved_index < 0) {
        return false;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        // Count only system header lines until we reach the requested saved system.
        if (line[0] != 'S' || line[1] != ' ') {
            continue;
        }

        current_index++;

        if (current_index != saved_index) {
            continue;
        }

        int body_count = 0;

        if (fgets(line, sizeof(line), file) == NULL || sscanf(line, "N %d", &body_count) != 1) {
            fclose(file);
            return false;
        }

        if (body_count > MAX_BODIES) {
            body_count = MAX_BODIES;
        }

        app->body_count = 0;

        for (int i = 0; i < body_count && fgets(line, sizeof(line), file) != NULL; i++) {
            if (parse_saved_body(line, &app->bodies[app->body_count])) {
                app->body_count++;
            }
        }

        // Loading resets the sim but keeps the user's current display settings.
        PathMode path_mode = simulator.path_mode;
        bool show_current_trajectory = simulator.show_current_trajectory;

        app->sim_time_seconds = 0.0;
        app->paused = true;
        app->add_body_requested = false;
        app->edit_body_requested = false;
        app->editing_body_index = -1;
        simulation_accumulator = 0.0f;

        simulator_reset(&simulator);
        simulator.path_mode = path_mode;
        simulator.show_current_trajectory = show_current_trajectory;
        simulator.active_template_index = -1;
        body_creator_init(&app->creator, app->screen_width, app->screen_height);
        body_creator_set_center_icon(&app->creator, simulator.origin_icon);
        app_refresh_saved_systems();

        fclose(file);
        return true;
    }

    fclose(file);
    return false;
}

// Deletes saved user system by copying all systems except the deleted one to a temporary file, then replacing the original file with the temporary one (refreshes templates menu after for instant changes)
static void app_delete_saved_system(int saved_index) {
    FILE *source = fopen(USER_SYSTEM_FILE, "r");
    FILE *target = NULL;
    char line[256];
    int current_index = -1;
    int skip_body_lines = 0;

    simulator.delete_saved_system_index = -1;

    if (source == NULL || saved_index < 0) {
        if (source != NULL) {
            fclose(source);
        }
        return;
    }

    target = fopen(USER_SYSTEM_FILE ".tmp", "w");
    if (target == NULL) {
        fclose(source);
        return;
    }

    while (fgets(line, sizeof(line), source) != NULL) {
        if (skip_body_lines > 0) {
            // Skip the body records that belong to the deleted saved system.
            skip_body_lines--;
            continue;
        }

        if (line[0] == 'S' && line[1] == ' ') {
            current_index++;

            if (current_index == saved_index) {
                // Read the following N line so we know how many B lines to skip.
                if (fgets(line, sizeof(line), source) != NULL) {
                    int body_count = 0;

                    if (sscanf(line, "N %d", &body_count) == 1 && body_count > 0) {
                        skip_body_lines = body_count;
                    }
                }
                continue;
            }
        }

        fputs(line, target);
    }

    fclose(source);
    fclose(target);

    // Replace the original only after the temporary file was written.
    if (remove(USER_SYSTEM_FILE) != 0 || rename(USER_SYSTEM_FILE ".tmp", USER_SYSTEM_FILE) != 0) {
        remove(USER_SYSTEM_FILE ".tmp");
        app_refresh_saved_systems();
        return;
    }

    if (simulator.requested_saved_system_index == saved_index) {
        simulator.requested_saved_system_index = -1;
    } else if (simulator.requested_saved_system_index > saved_index) {
        simulator.requested_saved_system_index--;
    }

    app_refresh_saved_systems();
}

// Deletes body and shifts relevant body indicies down by 1
static void app_delete_body(App *app, int index) {
    if (index < 0 || index >= app->body_count) {
        return;
    }

    // Compact the fixed body array after removing the requested index.
    for (int i = index; i < app->body_count - 1; i++) {
        app->bodies[i] = app->bodies[i + 1];
    }

    app->body_count--;

    // Any stored UI indices after the deleted body must shift with the array.
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

    if (app->editing_body_index == index) {
        app->editing_body_index = -1;
        app->edit_body_requested = false;
    } else if (app->editing_body_index > index) {
        app->editing_body_index--;
    }
}

// Takes template and loads bodies into app
static void app_load_template(App *app, int template_index) {
    if (simulator_template_get(template_index) == NULL) {
        simulator.requested_template_index = -1;
        return;
    }

    // Template changes reset physics state but preserve user-selected display options.
    PathMode path_mode = simulator.path_mode;
    bool show_current_trajectory = simulator.show_current_trajectory;

    app->body_count = simulator_template_copy_bodies(template_index, app->bodies, MAX_BODIES);
    app->sim_time_seconds = 0.0;
    app->paused = true;
    app->add_body_requested = false;
    app->edit_body_requested = false;
    app->editing_body_index = -1;
    simulation_accumulator = 0.0f;

    simulator_reset(&simulator);
    simulator.path_mode = path_mode;
    simulator.show_current_trajectory = show_current_trajectory;
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
        // App owns the real body array; the editor only provides draft values.
        app->bodies[app->body_count] = app_body_from_draft(draft);
        app->body_count++;
    }

    app->add_body_requested = false;
}

// Applies pending edit to body when clicking confirm in edit prompt
static void app_apply_pending_edit(App *app) {
    if (!app->edit_body_requested) {
        return;
    }

    if (app->editing_body_index >= 0 && app->editing_body_index < app->body_count) {
        // Rebuild the body so edited values get clean force/trail state.
        app->bodies[app->editing_body_index] = app_body_from_draft(&app->pending_body);
    }

    app->edit_body_requested = false;
    app->editing_body_index = -1;
}

// Start editing body by filling body creator draft with current body's data and opening body creator prompt
static void app_start_edit_body(App *app, int index) {
    if (index < 0 || index >= app->body_count) {
        simulator.edit_body_index = -1;
        return;
    }

    Body *body = &app->bodies[index];
    BodyDraft draft = {
        .mass = body->mass,
        // Editor works in screen space so the marker appears where the body is drawn.
        .position = simulator_world_to_screen(&simulator, body->position),
        .velocity = body->velocity,
        .color = body->color
    };

    snprintf(draft.name, sizeof(draft.name), "%s", body->name);
    app->editing_body_index = index;
    app->add_body_requested = false;
    app->edit_body_requested = false;
    body_creator_start_edit(&app->creator, draft);
    simulator.edit_body_index = -1;
}

// Centers camera on draft body (in body creator prompt)
static void app_handle_center_request(App *app) {
    if (!app->creator.center_requested) {
        return;
    }

    // Convert the draft marker to world space, center the camera there, then keep the marker visually centered.
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
    // Template/load requests return early because they replace most app state.
    if (simulator.requested_template_index >= 0) {
        app_load_template(app, simulator.requested_template_index);
        return true;
    }

    if (simulator.requested_saved_system_index >= 0) {
        app_load_saved_system(app, simulator.requested_saved_system_index);
        return true;
    }

    if (simulator.save_system_requested) {
        app_save_system(app);
    }

    // Remaining requests are independent and can be handled in the same frame.
    if (simulator.delete_saved_system_index >= 0) {
        app_delete_saved_system(simulator.delete_saved_system_index);
    }

    if (simulator.delete_body_index >= 0) {
        app_delete_body(app, simulator.delete_body_index);
        simulator.delete_body_index = -1;
    }

    if (simulator.edit_body_index >= 0) {
        app_start_edit_body(app, simulator.edit_body_index);
    }

    app_add_pending_body(app);
    app_apply_pending_edit(app);
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

    // UI speed multiplies elapsed real time before feeding the fixed-step accumulator.
    double scaled_dt = (double)app->dt * (double)app->sim_speed;

    app->sim_time_seconds += scaled_dt;
    simulation_accumulator += (float)scaled_dt;

    while (simulation_accumulator >= fixed_dt) {
        // Run as many fixed steps as needed to catch up without tying physics to FPS.
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
    app->sim_time_seconds = 0.0;
    app->paused = true;
    app->add_body_requested = false;
    app->edit_body_requested = false;
    app->editing_body_index = -1;

    simulator_init(&simulator);
    app_refresh_saved_systems();
    app_load_template(app, simulator.active_template_index);
}

// Main update function for each frame
void app_update(App *app) {
    // GetFrameTime is a raylib call that returns seconds since the last frame.
    app->dt = GetFrameTime();

    if (app_process_requests(app)) {
        return;
    }

    // Text boxes/modals block simulator shortcuts and camera input.
    simulator.input_blocked = body_creator_blocks_movement(&app->creator) || simulator.save_prompt_open;
    if (!simulator.input_blocked && IsKeyPressed(KEY_SPACE)) {
        app->paused = !app->paused;
    }

    app_update_simulation(app);
    simulator_update(&simulator, app->dt);
}

// Main draw function for each frame
void app_draw(App *app) {
    // While placing a body, simulator controls stay visible but should not mutate state.
    simulator.controls_blocked = app->creator.placing;

    simulator_draw(
        &simulator,
        app->bodies,
        app->body_count,
        &app->sim_speed,
        &app->paused,
        app->sim_time_seconds
    );

    body_creator_draw_preview(&app->creator);

    if (widget_button(layout_anchor(220, 40, LAYOUT_TOP_LEFT, 20, 180), "New Body") && !app->creator.placing) {
        app->editing_body_index = -1;
        body_creator_start(&app->creator, app->screen_width, app->screen_height);
    }

    if (body_creator_draw(&app->creator, app->screen_width, app->screen_height)) {
        bool was_editing = app->creator.editing;

        // Store the confirmed draft as a request; app_update/app_process_requests applies it.
        app->pending_body = app->creator.draft;
        app->add_body_requested = !was_editing;
        app->edit_body_requested = was_editing;
        app->creator.editing = false;
    }
}

// Clean up resources (just the origin button texture)
void app_deinit(App *app) {
    (void)app;
    simulator_deinit(&simulator);
}
