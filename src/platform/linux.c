#include "../configuration.h"
#include "../nyarlathotep.h"
#include "platform.h"

#ifdef linux

#include <unistd.h>
#include <pwd.h>
#include <dirent.h>

char* path_concat(const char* path, const char* element) {
	size_t sp = strlen(path);
	size_t se = strlen(element);
	char* np = malloc(sizeof(char) * (sp + se + 2));

	ASSERT_MEM(np);

	memcpy(np, path, sp);
	np[sp] = '/';
	memcpy(np + (sp + 1), element, se);
	np[sp + se + 1] = '\0';

	return np;
}

bool directory_exists(const char* dirpath) {
	DIR* dir = opendir(dirpath);
	if (dir) {
		closedir(dir);
		return true;
	} else {
		return false;
	}
}

char* get_pinion_workdir() {
	char* homedir = getenv("HOME");
	if (homedir == NULL) {
		uid_t uid = getuid();
		struct passwd* pwd = getpwuid(uid);
		homedir = pwd->pw_dir;

	}
	char* path = path_concat(homedir, current_configuration.workdir);
	ASSERT(directory_exists(path), "no home dir exists");
	return path;
}

const char* get_default_shell() {
	if (current_configuration.shell != NULL)
		return current_configuration.shell;

	uid_t uid = getuid();
	struct passwd* pwd = getpwuid(uid);
	if (pwd != NULL && pwd->pw_shell != NULL)
		return pwd->pw_shell;

	GENERAL_FAILURE("no shell exists");
	return NULL;
}

#endif
