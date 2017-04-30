#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "nyarlathotep.h"
#include "platform/platform.h"
#include "libds/aojls.h"
#include "libds/llist.h"
#include "utils.h"

typedef struct launcher {
	char* file;
	char* name;
	char* display_name;
	struct {
		char** names;
		size_t count;
	} categories;
	struct {
		char* program;
		struct {
			char** argv;
			int argc;
		} arguments;
	} launcher;
	struct {
		bool has_launch_config;
		long mods;
		uint32_t key;
	} launch_configuration;
} launcher_t;

#endif /*LAUNCHER_H*/
