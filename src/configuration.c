#include "pinion.h"
#include "configuration.h"

configuration_t current_configuration;

void init_configuration() {
	memset(&current_configuration, 0, sizeof(configuration_t));

	current_configuration.control_key_modifiers =
			WLC_BIT_MOD_CTRL | WLC_BIT_MOD_ALT;
	current_configuration.open_terminal_key = XKB_KEY_t;

	current_configuration.default_terminal = "weston-terminal";
}

const char* get_open_terminal_command() {
	return current_configuration.default_terminal;
}

bool open_terminal_pressed(uint32_t key, struct wlc_modifiers mods) {
	return key == current_configuration.open_terminal_key &&
			mods.mods == current_configuration.control_key_modifiers;
}
