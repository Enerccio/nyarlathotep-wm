#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

char* path_concat(const char* path, const char* element);

bool directory_exists(const char* dirpath);

char* get_pinion_workdir();
const char* get_default_shell();

#endif /* PLATFORM_H_ */
