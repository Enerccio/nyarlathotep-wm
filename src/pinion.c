#include "pinion.h"

void request_geometry(wlc_handle view, const struct wlc_geometry* geometry) {
	wlc_handle parent = wlc_view_get_parent(view);
	if (parent == 0) {
		// no effect to prevent user resizes
	} else {
		wlc_view_set_geometry(view, 0, geometry);
	}
}

int main(void) {

	wlc_log_set_handler(logger_callback);

	init_configuration();
	init_workspaces();

	wlc_set_output_created_cb(output_created);
	wlc_set_output_destroyed_cb(output_terminated);
	wlc_set_output_resolution_cb(resolution_changed);

	wlc_set_view_created_cb(view_created);

	wlc_set_keyboard_key_cb(keyboard_callback);

	wlc_set_view_request_geometry_cb(request_geometry);

	if (!wlc_init())
		return EXIT_FAILURE;

	wlc_run();

	return EXIT_SUCCESS;
}
