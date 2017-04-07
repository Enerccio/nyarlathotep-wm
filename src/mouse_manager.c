#include "pinion.h"

bool mouse_motion(wlc_handle view, uint32_t time,
		const struct wlc_point* pointer) {

	workspace_t* active_workspace = get_active_workspace();
	if (active_workspace != NULL) {
		return workspace_handle_mouse_motion(active_workspace,
				view, time, pointer);
	}

	wlc_pointer_set_position(pointer);
	return false;
}
