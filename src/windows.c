#include "nyarlathotep.h"

bool view_created(wlc_handle view) {
	workspace_t* workspace = get_workspace_for_view(view);
	if (workspace == NULL)
		return false;
	workspace_set_main_window(workspace, view);
	return true;
}

void view_focus_change(wlc_handle view, bool focus) {
	workspace_t* workspace = get_workspace_for_view(view);
	if (workspace == NULL)
		return;

	if (focus) {
		workspace_view_got_focus(workspace, view);
	} else {
		wlc_view_set_state(view, WLC_BIT_ACTIVATED, false);
	}
}
