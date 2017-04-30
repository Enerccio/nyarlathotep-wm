#include "configuration.h"
#include "nyarlathotep.h"
#include "platform/platform.h"

configuration_t current_configuration;

void init_configuration() {
	LOG_DEBUG("loading initial configuration");

	memset(&current_configuration, 0, sizeof(configuration_t));

	current_configuration.config_file_loc = "/home/enerccio/.nyarla/thotep.conf";
	current_configuration.cursor_image_source = "/home/enerccio/.nyarla/cursor.png";
	current_configuration.cursor_offset[0] = 2;
	current_configuration.cursor_offset[1] = 2;

	current_configuration.control_key_modifiers =
			WLC_BIT_MOD_CTRL | WLC_BIT_MOD_ALT;
	current_configuration.move_window_to_background = XKB_KEY_m;

	// TODO
	current_configuration.background = NULL;

	current_configuration.shell = NULL;
	current_configuration.workdir = ".nyarla";
	current_configuration.execution_script = "thotep.rc";
	current_configuration.lancher_dir = "launchers";
	current_configuration.theme = "default.theme";
}

/*
static int copy_int(int i) {
	return i;
}

static float copy_float(float f) {
	return f;
}
*/

#define VALUE_READ(config, read_op, copy_op, where, type, path ) do { \
	type v;\
	if (read_op(config, path, &v) == CONFIG_TRUE) {\
		LOG_TRACE("loading config key: " path);\
		c->where = copy_op(v);\
	}\
} while (0)

#define STRING_VALUE_READ(where, path) \
	VALUE_READ(config, config_lookup_string, copy_string, where, const char*, path)
#define FLOAT_VALUE_READ(where, path) \
	VALUE_READ(config, config_lookup_float, copy_float, where, float, path)
#define INT_VALUE_READ(where, path) \
	VALUE_READ(config, config_lookup_int, copy_int, where, int, path)

void load_configuration_from_cfg(config_t* config) {
	// TODO arrays, float arrays
	LOG_DEBUG("loading configuration from config file");

	configuration_t* c = &current_configuration;

	STRING_VALUE_READ(cursor_image_source, "cursor");

	STRING_VALUE_READ(default_terminal, "default_terminal");
	STRING_VALUE_READ(workdir, "workdir");
	STRING_VALUE_READ(execution_script, "initrc");
	STRING_VALUE_READ(lancher_dir, "launcher_dir");

	STRING_VALUE_READ(background, "background");
}

const char* get_cursor_image() {
	return current_configuration.cursor_image_source;
}

const int* get_cursor_offset() {
	return current_configuration.cursor_offset;
}

const char* get_open_terminal_command() {
	return current_configuration.default_terminal;
}

const char* get_background() {
	return current_configuration.background;
}

const char* get_active_theme() {
	return path_concat(path_concat(get_nyarlathotep_workdir(), "themes"), current_configuration.theme);
}

bool window_move_to_background_pressed(uint32_t key, struct wlc_modifiers mods) {
	return key == current_configuration.move_window_to_background &&
				mods.mods == current_configuration.control_key_modifiers;
}

const char* get_launcher_dir() {
	if (path_is_absolute(current_configuration.lancher_dir)) {
		return current_configuration.lancher_dir;
	}
	return path_concat(get_nyarlathotep_workdir(), current_configuration.lancher_dir);
}
