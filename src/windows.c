#include "nyarlathotep.h"

bool view_created(wlc_handle view) {
	LOG_DEBUG("view created");

	workspace_t* workspace = get_workspace_for_view(view);
	if (workspace == NULL)
		return false;
	workspace_set_main_window(workspace, view);
	return true;
}

void view_focus_change(wlc_handle view, bool focus) {
	LOG_DEBUG("view focus changed");

	workspace_t* workspace = get_workspace_for_view(view);
	if (workspace == NULL)
		return;

	if (focus) {
		workspace_view_got_focus(workspace, view);
	} else {
		wlc_view_set_state(view, WLC_BIT_ACTIVATED, false);
	}
}

void view_destroyed(wlc_handle view) {
	LOG_DEBUG("view destroyed");

	workspace_t* workspace = get_workspace_for_view(view);
	if (workspace == NULL)
		return;
	workspace_view_destroyed(workspace, view);
}

void state_change(wlc_handle view, enum wlc_view_state_bit state, bool change) {
	LOG_DEBUG("view state changed");

	workspace_t* workspace = get_workspace_for_view(view);
	if (workspace == NULL)
		return;
	if (state & WLC_BIT_MAXIMIZED) {
		workspace_maximize_request(workspace, view, change);
	}
}

void request_geometry(wlc_handle view, const struct wlc_geometry* geometry) {
	workspace_t* workspace = get_workspace_for_view(view);
	if (workspace == NULL)
		return;

	wlc_handle parent = wlc_view_get_parent(view);
	if (parent == 0) {
		// no effect to prevent user resizes
	} else {
		struct wlc_geometry g;
		g = *geometry;

		const struct wlc_geometry* anchor_rect = wlc_view_positioner_get_anchor_rect(view);
		if (anchor_rect) {
			const struct wlc_size* request = wlc_view_positioner_get_size(view);
			if (request->h > 0 && request->w > 0) {
				g.size = *request;
			}
			g.origin = anchor_rect->origin;
		}

		handle_float_view_geometry(workspace, view, &g);
	}
}
