#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <xkbcommon/xkbcommon-keysyms.h>

typedef struct configuration {
	uint32_t control_key_modifiers;
	uint32_t open_terminal_key;

	const char* default_terminal;
	const char* background;

	float window_list_color[4];
	float window_list_selection_color[4];
	int window_list_sizes[2];
} configuration_t;

#endif /* CONFIGURATION_H_ */
