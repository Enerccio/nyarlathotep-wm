#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <stdint.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <libconfig.h>

typedef struct configuration {
	const char* config_file_loc;
	const char* cursor_image_source;
	int32_t cursor_offset[2];

	uint32_t control_key_modifiers;
	uint32_t move_window_to_background;

	const char* shell;
	const char* workdir;
	const char* execution_script;
	const char* lancher_dir;

	const char* default_terminal;
	const char* background;
	const char* theme;
} configuration_t;

extern configuration_t current_configuration;

void load_configuration_from_cfg(config_t* config);

#endif /* CONFIGURATION_H_ */
