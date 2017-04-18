#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "../libds/llist.h"

void init_platform();
void init_platform_compositor();

void platform_dependent_logging(enum logging_type, const char* message);
char* path_concat(const char* path, const char* element);
bool path_is_absolute(const char* path);
bool directory_exists(const char* dirpath);
char* get_nyarlathotep_workdir();
const char* get_default_shell();
void get_directory_content(const char* directory, list_t* result);

enum notify_type {
	DELETED, CREATED, MODIFIED
};

struct notify_event {
	void* data;
	char* element;
	char* register_path;
	bool isdir;
	enum notify_type type;
};

void register_notifier(const char* path, void(callback)(const struct notify_event* event), void* data);
void unregister_notifier(const char* path);
void unregister_notifier_callback(const char* path, void(callback)(struct notify_event event));
void unregister_notifier_callback_data(const char* path, void(callback)(struct notify_event event), void* data);

#endif /* PLATFORM_H_ */
