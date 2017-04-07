/*
 * Grouped header for all subsystems.
 *
 * All subsystems must import this header.
 */
#ifndef PINION_H_
#define PINION_H_

#include <stdio.h>
#include <stdlib.h>

#include <wlc/wlc.h>

#define NO_RENDER_MASK (0)
#define RENDER_MASK (1)

typedef struct workspace workspace_t;

// log.c
void logger_callback(enum wlc_log_type type, const char *str);
// log.c ends

// keyboard_manager.c
bool keyboard_callback(wlc_handle view, uint32_t time,
		const struct wlc_modifiers*, uint32_t key, enum wlc_key_state);
bool keyboard_scroll(wlc_handle view, uint32_t time, const struct wlc_modifiers*,
		uint8_t axis_bits, double amount[2]);
// keyboard_manager.c ends

// mouse_manager.c
bool mouse_motion(wlc_handle view, uint32_t time,
		const struct wlc_point* pointer);
// mouse_manager.c ends

// configuration.c
void init_configuration();

const char* get_open_terminal_command();
const char* get_background();
const float* get_window_list_color();
int get_side_window_height();
int get_size_window_offset();

bool open_terminal_pressed(uint32_t key, struct wlc_modifiers mods);
// configuration.c ends

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
// windows.c ends

// render.c
void init_render();
void context_open(wlc_handle output);
void context_closed(wlc_handle output);
void custom_render(wlc_handle output);
// render.c ends

#endif /* PINION_H_ */
