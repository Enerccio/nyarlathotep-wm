#include "libds/hmap.h"
#include <wlc/wlc-render.h>
#include "workspace.h"
#include "nyarlathotep.h"
#include "utils.h"

/* Holds output->workspace hash map */
hash_table_t* workspaces;

/**
 * Initializes workspace subsystem.
 */
void init_workspaces() {
	workspaces = create_uint64_table();
}

/**
 * Called when output is created by wlc.
 *
 * Handles creation of workspace for each output.
 */
bool output_created(wlc_handle output) {
	if (table_contains(workspaces, (void*)output)) {
		return true;
	}

	workspace_t* workspace = create_workspace(output);

	table_set(workspaces, (void*)output, (void*)workspace);
	wlc_output_set_mask(output, RENDER_MASK);
	return true;
}

/**
 * Called when output terminates.
 *
 * Workspace for that output is terminated.
 */
void output_terminated(wlc_handle output) {
	workspace_t* workspace = table_get(workspaces, (void*)output);
	if (workspace != NULL) {
		terminate_workspace(workspace);
		table_remove(workspaces, (void*)output); // clear the
												 //  association from table
	}
}

void resolution_changed(wlc_handle output,
		const struct wlc_size *from, const struct wlc_size *to) {
	workspace_t* workspace = table_get(workspaces, (void*)output);
	if (workspace == NULL) {
		output_created(output);
		workspace = table_get(workspaces, (void*)output);
	}
	if (workspace != NULL) {
		workspace->w = to->w;
		workspace->h = to->h;
		resize_workspace(workspace);
	}
}

/**
 * creates and initializes workspace
 *
 * returns NULL on failure (memory insufficient)
 */
workspace_t* create_workspace(wlc_handle output) {
	workspace_t* workspace = malloc(sizeof(workspace_t));
	ASSERT_MEM(workspace);
	memset(workspace, 0, sizeof(workspace_t));

	workspace->output = output;
	workspace->hidden_windows = create_list();
	workspace->floating_windows = create_list();

	ASSERT_MEM(workspace->floating_windows);
	ASSERT_MEM(workspace->hidden_windows);

	return workspace;
}

/**
 * terminates workspace
 *
 * frees the underlying memory structures (deep clean)
 */
void terminate_workspace(workspace_t* workspace) {
	// TODO
	free_list(workspace->hidden_windows);
	free(workspace);
}

/**
 * returns active workspace for focused output or null
 */
workspace_t* get_active_workspace() {
	wlc_handle output = wlc_get_focused_output();
	if (output == 0)
		return NULL;
	return table_get(workspaces, (void*)output);
}

/**
 * returns active workspace for output
 */
workspace_t* get_workspace_for_output(wlc_handle output) {
	return table_get(workspaces, (void*)output);
}

/**
 * returns active workspace for selected view
 *
 * reparents to focused output, if it has none
 */
workspace_t* get_workspace_for_view(wlc_handle view) {
	wlc_handle output = wlc_view_get_output(view);
	if (output == 0) {
		output = wlc_get_focused_output();
		wlc_view_set_output(view, output);
	}
	if (output == 0)
		return NULL;
	return table_get(workspaces, (void*)output);
}

void workspace_view_destroyed(workspace_t* workspace,
		wlc_handle view) {
	if (workspace->move_over_view == view) {
		workspace->move_over_view = 0;
	}

	if (workspace->main_view == view) {
		workspace->main_view = 0;
		wlc_view_focus(0);
	} else {
		list_remove((void*)view, workspace->floating_windows);
		list_remove((void*)view, workspace->hidden_windows);
		if (is_parent_view(workspace->main_view, view)) {
			wlc_handle parent = wlc_view_get_parent(view);
			wlc_view_focus(parent);
		}
		// TODO: pinned
	}
	wlc_output_schedule_render(workspace->output);
}

void workspace_maximize_request(workspace_t* workspace,
		wlc_handle view, bool change) {
	if (view == workspace->main_view) {
		// ignore all changes
		return;
	}
	if (list_contains(workspace->floating_windows, (void*)view)) {
		wlc_view_set_state(view, WLC_BIT_MAXIMIZED, change);
		return;
	}
	// TODO: pinned windows
}

void workspace_set_view_hidden(workspace_t* workspace,
		wlc_handle view) {
	struct wlc_geometry geometry;
	geometry.origin.x = 0;
	geometry.origin.y = 0;
	geometry.size.w = workspace->w;
	geometry.size.h = workspace->h;

	wlc_view_set_geometry(view, 0, &geometry);
	wlc_view_set_mask(view, NO_RENDER_MASK);
	wlc_view_set_state(view, WLC_BIT_MAXIMIZED, false);
	list_push_right(workspace->hidden_windows, (void*)view);
	wlc_view_send_below(view, workspace->main_view);

	if (workspace->window_list_show_width > 0)
		wlc_output_schedule_render(workspace->output); // to redraw sidebar
}

void workspace_set_main_window(workspace_t* workspace,
		wlc_handle view) {
	if (workspace->fullscreen_view != 0) {
		// keep fullscreening view on fullscreen
		workspace_set_view_hidden(workspace, view);
	} else {
		if (wlc_view_get_parent(view) != 0) {
			// floating subwindow
			// TODO handle this shit
			list_push_right(workspace->floating_windows, (void*)view);
			if (workspace->main_view != 0 && is_parent_view(workspace->main_view, view)) {
				wlc_view_set_mask(view, RENDER_MASK);
				wlc_view_bring_above(view, workspace->main_view);
				wlc_view_focus(view);
			}
		} else {
			wlc_handle topview = workspace->main_view;
			workspace->main_view = view;

			if (topview != 0) {
				// there is already a main view, move it to the background
				workspace_set_view_hidden(workspace, topview);
			}

			struct wlc_geometry geometry;
			geometry.origin.x = 0;
			geometry.origin.y = 0;
			geometry.size.w = workspace->w;
			geometry.size.h = workspace->h;

			wlc_view_set_geometry(view, 0, &geometry);
			wlc_view_set_mask(view, RENDER_MASK);
			wlc_view_set_state(view, WLC_BIT_MAXIMIZED, true);
			wlc_view_bring_to_front(view);
			wlc_view_focus(view);
		}
	}
}

void resize_workspace(workspace_t* workspace) {

	struct wlc_geometry geometry;
	geometry.origin.x = 0;
	geometry.origin.y = 0;
	geometry.size.w = workspace->w;
	geometry.size.h = workspace->h;

	if (workspace->main_view != 0) {
		wlc_view_set_geometry(workspace->main_view, 0, &geometry);
	}

	workspace->window_list_show_width = 0;
	workspace->window_list_total_width = workspace->w * 0.15;
}

bool open_window_list_animator(void* data) {
	workspace_t* workspace = (workspace_t*)data;
	if (workspace->window_list_force_hide) {
		close_window_list(workspace);
		return true;
	}

	int byfar = workspace->window_list_total_width / (500/12);
	if (byfar <= 0)
		byfar = 1; // minimum increase
	if (workspace->window_list_show_width + byfar > workspace->window_list_total_width) {
		workspace->window_list_show_width = workspace->window_list_total_width;
	} else {
		workspace->window_list_show_width += byfar;
	}

	wlc_output_schedule_render(workspace->output);

	if (workspace->window_list_show_width == workspace->window_list_total_width) {
		workspace->window_list_opening = false;
		return true;
	} else {
		return false;
	}
}

void open_window_list(workspace_t* workspace) {
	if (workspace->window_list_opening)
		return;

	wlc_view_focus(0); // remove focus
	workspace->window_list_opening = true;
	workspace->window_list_force_hide = false;
	if (list_size(workspace->hidden_windows) > 0) {
		workspace->window_list_selected_view = (wlc_handle)list_get_left(workspace->hidden_windows);
	}

	register_animation(open_window_list_animator, workspace, 12);
}

void close_window_list(workspace_t* workspace) {
	workspace->window_list_show_width = 0;
	workspace->window_list_force_hide = false;
	workspace->window_list_opening = false;
	workspace->window_list_scroll_offset = 0;
	workspace->window_list_selected_view = 0;
}

void workspace_view_got_focus(workspace_t* workspace,
		wlc_handle view) {

	workspace->focused_view = view;

	if (workspace->main_view == view) {
		close_window_list(workspace);
		wlc_view_focus(view);
		wlc_view_set_state(view, WLC_BIT_ACTIVATED, true);
		return;
	} else {
		if (list_contains(workspace->floating_windows, (void*)view)) {
			// TODO: handle pinned subviews
			if (is_parent_view(workspace->main_view, view)) {
				wlc_view_focus(view);
				wlc_view_set_state(view, WLC_BIT_ACTIVATED, true);
				return;
			}
			wlc_view_set_state(view, WLC_BIT_ACTIVATED, false);
			return;
		}
		// TODO: pinned windows

		// ignore, other windows can get focus
	}
	wlc_view_set_state(view, WLC_BIT_ACTIVATED, false);
}

void move_view_to_background(workspace_t* workspace) {
	if (workspace->focused_view) {
		if (workspace->focused_view == workspace->fullscreen_view) {
			// TODO: handle
		} else if (workspace->main_view == workspace->focused_view) {
			workspace->main_view = 0;
		} else {
			// TODO: handle pinned windows
		}

		wlc_view_set_state(workspace->focused_view, WLC_BIT_ACTIVATED, false);
		wlc_view_set_mask(workspace->focused_view, NO_RENDER_MASK);
		list_push_left(workspace->hidden_windows, (void*)workspace->focused_view);
		workspace->focused_view = 0;
	}
}

void handle_float_view_geometry(workspace_t* workspace, wlc_handle view,
		const struct wlc_geometry* geometry) {

	struct wlc_geometry g;
	g.origin.x = geometry->origin.x;
	g.origin.y = geometry->origin.y;
	g.size.w = CLAMP(geometry->size.w, workspace->w);
	g.size.h = CLAMP(geometry->size.h, workspace->h);

	wlc_view_set_geometry(view, 0, &g);
}

bool workspace_minimize_request(workspace_t* workspace,
		wlc_handle view, bool minimize) {
	if (minimize && view == workspace->main_view) {
		move_view_to_background(workspace);
	} // TODO: floating windows
	return false;
}

/**
 * Handles all key inputs related to workspace
 */
bool workspace_handle_key_input(workspace_t* workspace,
		wlc_handle view, uint32_t time,
		const struct wlc_modifiers* mods, uint32_t key,
		enum wlc_key_state state) {

	uint32_t symkey = wlc_keyboard_get_keysym_for_key(key, mods);

	if (window_move_to_background_pressed(symkey, *mods)) {
		if (state == WLC_KEY_STATE_PRESSED) {
			move_view_to_background(workspace);
		}
		return true;
	}

	if (state == WLC_KEY_STATE_PRESSED) {
		return launcher_execute_key_pressed(key, *mods);
	}

	return false;
}

bool workspace_handle_mouse_motion(workspace_t* workspace, wlc_handle view, uint32_t time,
		const struct wlc_point* pointer) {
	workspace->px = pointer->x;
	workspace->py = pointer->y;
	workspace->move_over_view = view;

	if (pointer->x >= workspace->w-workspace->window_list_show_width) {
		workspace->move_over_view = 0;
		return false;
	}

	if (pointer->x >= workspace->w-1) {
		open_window_list(workspace);
		return true;
	}
	return false;
}

bool workspace_handle_scroll(workspace_t* workspace, wlc_handle view, uint32_t time,
		const struct wlc_modifiers* mods,
		uint8_t axis_bits, double amount[2]) {

	if (workspace->px >= (workspace->w - workspace->window_list_show_width)) {
		if (axis_bits & WLC_SCROLL_AXIS_VERTICAL) {
			workspace->window_list_scroll_offset += amount[0];
			wlc_output_schedule_render(workspace->output);
			return true;
		}
	}

	return false;
}
