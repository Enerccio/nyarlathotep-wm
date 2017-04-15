#include "nyarlathotep.h"

#include "configuration.h"
#include "platform/platform.h"

void load_config() {
	// TODO: arguments
	config_t config;
	config_init(&config);

	config_read_file(&config, current_configuration.config_file_loc);
	load_configuration_from_cfg(&config);

	config_destroy(&config);
}

void compositor_ready() {
	char* wd = get_pinion_workdir();
	if (current_configuration.execution_script != NULL &&
			strcmp(current_configuration.execution_script, "") != 0) {
		LOG_INFO("starting initrc file");

		char* rc = path_concat(wd, current_configuration.execution_script);
		const char* args[2];
		args[0] = get_default_shell();
		args[1] = rc;
		wlc_exec(args[0], (char* const*)args);
	}
}

int main(int argc, char** argv) {
	// TODO: arguments
	LOG_INFO("nyarlathotep window manager starting");

	wlc_log_set_handler(logger_callback);

	init_configuration();
	load_config();
	init_workspaces();
	init_render();

	wlc_set_compositor_ready_cb(compositor_ready);

	wlc_set_output_created_cb(output_created);
	wlc_set_output_destroyed_cb(output_terminated);
	wlc_set_output_resolution_cb(resolution_changed);

	wlc_set_view_created_cb(view_created);
	wlc_set_view_request_geometry_cb(request_geometry);
	wlc_set_view_focus_cb(view_focus_change);
	wlc_set_view_destroyed_cb(view_destroyed);
	wlc_set_view_request_state_cb(state_change);

	wlc_set_keyboard_key_cb(keyboard_callback);
	wlc_set_pointer_motion_cb(mouse_motion);
	wlc_set_pointer_scroll_cb(mouse_scroll);
	wlc_set_pointer_button_cb(mouse_click);
	wlc_set_touch_cb(mouse_touch);

	wlc_set_output_context_created_cb(context_open);
	wlc_set_output_context_destroyed_cb(context_closed);
	wlc_set_output_render_post_cb(custom_render);

	if (!wlc_init())
		return EXIT_FAILURE;

	LOG_INFO("starting wlc event loop");
	wlc_run();

	return EXIT_SUCCESS;
}
