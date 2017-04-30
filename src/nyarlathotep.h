/*
 * Grouped header for all subsystems.
 *
 * All subsystems must import this header.
 */
#ifndef NYARLATHOTEP_H_
#define NYARLATHOTEP_H_

#include <stdio.h>
#include <stdlib.h>

#include <wlc/wlc.h>
#include <wayland-server.h>

#include "macros.h"
#include "utils.h"

typedef struct workspace workspace_t;
typedef struct launcher launcher_t;

// log.c
void logger_callback(enum wlc_log_type type, const char *str);
// log.c ends

// keyboard_manager.c
bool keyboard_callback(wlc_handle view, uint32_t time,
		const struct wlc_modifiers*, uint32_t key, enum wlc_key_state);
// keyboard_manager.c ends

// mouse_manager.c
bool mouse_motion(wlc_handle view, uint32_t time,
		const struct wlc_point* pointer);
bool mouse_scroll(wlc_handle view, uint32_t time, const struct wlc_modifiers*,
		uint8_t axis_bits, double amount[2]);
bool mouse_click(wlc_handle view, uint32_t time, const struct wlc_modifiers* mods,
		uint32_t button, enum wlc_button_state state, const struct wlc_point*);
bool mouse_touch(wlc_handle view, uint32_t time,
		const struct wlc_modifiers* mods, enum wlc_touch_type type, int32_t slot,
		const struct wlc_point* point);
// mouse_manager.c ends

// configuration.c
void init_configuration();

const char* get_cursor_image();
const int* get_cursor_offset();
const char* get_background();
const char* get_launcher_dir();
const char* get_active_theme();

bool window_move_to_background_pressed(uint32_t key, struct wlc_modifiers mods);
// configuration.c ends

// launcher.c
void init_launchers(const char* from);
bool launcher_execute_key_pressed(uint32_t key, struct wlc_modifiers mods);
bool launch_launcher(launcher_t* launcher);
// launcher.c ends

// theme.c
void set_active_theme(const char* file);
struct rgb_color get_window_list_color();
struct rgb_color get_window_list_selection_color();
int get_side_window_height();
int get_size_window_offset();
// theme.c ends

// animator.c
typedef bool(*animation_request)(void* data);
void register_animation(animation_request request, void* data, int delay);
// animator.c ends

// workspace.c
void init_workspaces();

bool output_created(wlc_handle output);
void output_terminated(wlc_handle output);
void resolution_changed(wlc_handle output,
		const struct wlc_size *from, const struct wlc_size *to);

workspace_t* get_active_workspace();
workspace_t* get_workspace_for_output(wlc_handle output);
workspace_t* get_workspace_for_view(wlc_handle view);

void workspace_set_view_hidden(workspace_t* workspace,
		wlc_handle view);
void workspace_set_main_window(workspace_t* workspace,
		wlc_handle view);
void workspace_view_destroyed(workspace_t* workspace,
		wlc_handle view);
void handle_float_view_geometry(workspace_t* workspace,
		wlc_handle view, const struct wlc_geometry* geometry);

void workspace_view_got_focus(workspace_t* workspace,
		wlc_handle view);
void workspace_maximize_request(workspace_t* workspace,
		wlc_handle view, bool change);
bool workspace_minimize_request(workspace_t* workspace,
		wlc_handle view, bool change);

bool workspace_handle_key_input(workspace_t* workspace,
		wlc_handle view, uint32_t time,
		const struct wlc_modifiers* mods, uint32_t key,
		enum wlc_key_state state);
bool workspace_handle_mouse_motion(workspace_t* workspace, wlc_handle view,
		uint32_t time,
		const struct wlc_point* pointer);
bool workspace_handle_scroll(workspace_t* workspace, wlc_handle view, uint32_t time,
		const struct wlc_modifiers*,
		uint8_t axis_bits, double amount[2]);
// workspace.c ends

// windows.c
bool view_created(wlc_handle view);
void view_focus_change(wlc_handle view, bool focus);
void view_destroyed(wlc_handle view);
void state_change(wlc_handle view, enum wlc_view_state_bit state, bool change);
void request_geometry(wlc_handle view, const struct wlc_geometry* geometry);
bool minimize_request(wlc_handle view, bool minimize);
// windows.c ends

// render.c
void init_render();
void context_open(wlc_handle output);
void context_closed(wlc_handle output);
bool custom_render(wlc_handle output);
// render.c ends

#endif /* NYARLATHOTEP_H_ */
