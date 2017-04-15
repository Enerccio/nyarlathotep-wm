#include "nyarlathotep.h"
#include "workspace.h"

bool mouse_motion(wlc_handle view, uint32_t time,
		const struct wlc_point* pointer) {
	LOG_TRACE("mouse motion triggered");
	wlc_pointer_set_position(pointer);

	workspace_t* active_workspace = get_active_workspace();
	if (active_workspace != NULL) {
		return workspace_handle_mouse_motion(active_workspace,
				view, time, pointer);
	}

	return false;
}

bool mouse_scroll(wlc_handle view, uint32_t time, const struct wlc_modifiers* mods,
		uint8_t axis_bits, double amount[2]) {
	LOG_TRACE("mouse scroll triggered");

	workspace_t* active_workspace = get_active_workspace();
	if (active_workspace != NULL) {
		return workspace_handle_scroll(active_workspace,
				view, time, mods, axis_bits, amount);
	}

	return false;
}

bool mouse_click(wlc_handle view, uint32_t time, const struct wlc_modifiers* mods,
		uint32_t button, enum wlc_button_state state, const struct wlc_point* point) {
	LOG_DEBUG("mouse click triggered");

	if (state == WLC_BUTTON_STATE_PRESSED)
		wlc_view_focus(view);

	return false;
}

bool mouse_touch(wlc_handle view, uint32_t time,
		const struct wlc_modifiers* mods, enum wlc_touch_type type, int32_t slot,
		const struct wlc_point* point) {

	return false;
}
