#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <xkbcommon/xkbcommon-keysyms.h>

typedef struct configuration {
	uint32_t control_key_modifiers;
	uint32_t open_terminal_key;

	const char* default_terminal;
	const char* background;
} configuration_t;

#endif /* CONFIGURATION_H_ */
