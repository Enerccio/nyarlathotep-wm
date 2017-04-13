#include "configuration.h"
#include "nyarlathotep.h"

configuration_t current_configuration;

void init_configuration() {
	memset(&current_configuration, 0, sizeof(configuration_t));

	current_configuration.config_file_loc = "/home/enerccio/.nyarla/thotep.conf";
	current_configuration.cursor_image_source = "/home/enerccio/.nyarla/cursor.png";

	current_configuration.control_key_modifiers =
			WLC_BIT_MOD_CTRL | WLC_BIT_MOD_ALT;
	current_configuration.open_terminal_key = XKB_KEY_t;

	current_configuration.default_terminal = "weston-terminal";
	// TODO
	current_configuration.background = NULL;

	current_configuration.shell = NULL;
	current_configuration.workdir = ".nyarla";
	current_configuration.execution_script = "thotep.rc";

	current_configuration.window_list_color[0] = 1.0;
	current_configuration.window_list_color[1] = 0.0;
	current_configuration.window_list_color[2] = 0.0;
	current_configuration.window_list_color[3] = 1.0;

	current_configuration.window_list_selection_color[0] = 0.0;
	current_configuration.window_list_selection_color[1] = 1.0;
	current_configuration.window_list_selection_color[2] = 0.0;
	current_configuration.window_list_selection_color[3] = 1.0;

	current_configuration.window_list_sizes[0] = 100;
	current_configuration.window_list_sizes[1] = current_configuration.window_list_sizes[0] + 30;
}

static char* copy_string(const char* string) {
	char* dest = malloc(sizeof(char)*(strlen(string)+1));
	ASSERT_MEM(dest);
	strcpy(dest, string);
	return dest;
}

static int copy_int(int i) {
	return i;
}

static float copy_float(float f) {
	return f;
}

#define VALUE_READ(config, read_op, copy_op, where, type, path ) do { \
	type v;\
	if (read_op(config, path, &v) == CONFIG_TRUE) {\
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
	configuration_t* c = &current_configuration;

	STRING_VALUE_READ(cursor_image_source, "cursor");

	STRING_VALUE_READ(default_terminal, "default_terminal");
	STRING_VALUE_READ(workdir, "workdir");
	STRING_VALUE_READ(execution_script, "initrc");

	STRING_VALUE_READ(background, "background");
	INT_VALUE_READ(window_list_sizes[0], "window_list_window_size");
}

const char* get_cursor_image() {
	return current_configuration.cursor_image_source;
}

const char* get_open_terminal_command() {
	return current_configuration.default_terminal;
}

const char* get_background() {
	return current_configuration.background;
}

const float* get_window_list_color() {
	return current_configuration.window_list_color;
}

const float* get_window_list_selection_color() {
	return current_configuration.window_list_selection_color;
}

bool open_terminal_pressed(uint32_t key, struct wlc_modifiers mods) {
	return key == current_configuration.open_terminal_key &&
			mods.mods == current_configuration.control_key_modifiers;
}

int get_side_window_height() {
	return current_configuration.window_list_sizes[0];
}

int get_size_window_offset() {
	return current_configuration.window_list_sizes[1];
}
