#include "pinion.h"
#include "configuration.h"

configuration_t current_configuration;

void init_configuration() {
	memset(&current_configuration, 0, sizeof(configuration_t));

	current_configuration.control_key_modifiers =
			WLC_BIT_MOD_CTRL | WLC_BIT_MOD_ALT;
	current_configuration.open_terminal_key = XKB_KEY_t;

	current_configuration.default_terminal = "weston-terminal";
	// TODO
	current_configuration.background = "/home/enerccio/background.jpg";

	current_configuration.window_list_color[0] = 1.0;
	current_configuration.window_list_color[1] = 0.0;
	current_configuration.window_list_color[2] = 0.0;
	current_configuration.window_list_color[3] = 1.0;
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

bool open_terminal_pressed(uint32_t key, struct wlc_modifiers mods) {
	return key == current_configuration.open_terminal_key &&
			mods.mods == current_configuration.control_key_modifiers;
}
