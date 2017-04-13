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
