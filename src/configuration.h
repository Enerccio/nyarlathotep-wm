#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <stdint.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <libconfig.h>

typedef struct configuration {
	const char* config_file_loc;

	uint32_t control_key_modifiers;
	uint32_t open_terminal_key;

	const char* shell;
	const char* workdir;
	const char* execution_script;

	const char* default_terminal;
	const char* background;

	float window_list_color[4];
	float window_list_selection_color[4];
	int window_list_sizes[2];
} configuration_t;

extern configuration_t current_configuration;

void load_configuration_from_cfg(config_t* config);

#endif /* CONFIGURATION_H_ */
