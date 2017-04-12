#include "nyarlathotep.h"

bool keyboard_callback(wlc_handle view, uint32_t time,
		const struct wlc_modifiers* mods, uint32_t key,
		enum wlc_key_state state) {

	workspace_t* active_workspace = get_active_workspace();
	if (active_workspace != NULL) {
		return workspace_handle_key_input(active_workspace,
				view, time, mods, key, state);
	}

	return false;
}
