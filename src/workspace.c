#include "libds/hmap.h"
#include "pinion.h"
#include "workspace.h"

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
	if (workspace == NULL)
		return false; // failed to allocate memory
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
	if (workspace == NULL)
		return NULL;

	memset(workspace, 0, sizeof(workspace_t));

	workspace->output = output;
	workspace->hidden_windows = create_list();

	if (workspace->hidden_windows == NULL) {
		free(workspace);
		return NULL;
	}

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
		} else {
			wlc_handle topview = workspace->main_view;
			workspace->main_view = view;

			if (workspace->main_view != 0) {
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
			wlc_view_focus(view);
		}
	}
}

void resize_workspace(workspace_t* workspace) {
	// TODO
}

/**
 * Handles all key inputs related to workspace
 */
bool workspace_handle_key_input(workspace_t* workspace,
		wlc_handle view, uint32_t time,
		const struct wlc_modifiers* mods, uint32_t key,
		enum wlc_key_state state) {

	uint32_t symkey = wlc_keyboard_get_keysym_for_key(key, mods);

	if (open_terminal_pressed(symkey, *mods)) {
		if (state == WLC_KEY_STATE_PRESSED) {
			const char* terminal = get_open_terminal_command();
			const char* args[2];
			args[0] = terminal;
			args[1] = NULL;
			wlc_exec(terminal, (char * const*)&args);
		}
		return true;
	}

	return false;
}
