#include "../configuration.h"
#include "../nyarlathotep.h"
#include "platform.h"
#include "../utils.h"

#ifdef linux

#include <unistd.h>
#include <pwd.h>
#include <dirent.h>

#ifdef journal_d
#include <syslog.h>
#endif

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

#ifdef journal_d
int convert_log_type(enum logging_type ltype) {
	switch (ltype) {
		case FATAL: return LOG_CRIT;
		case ERROR: return LOG_ERR;
		case WARN: return LOG_WARNING;
		case DEBUG: return LOG_DEBUG;
		case TRACE: return LOG_DEBUG;
		default:
			break;
	}
	return LOG_INFO;
}
#endif

void platform_dependent_logging(enum logging_type ltype, const char* message) {
	const char* type = logging_type_str(ltype);

#ifdef journal_d
	syslog(convert_log_type(ltype), message);
#endif

	// sys err logging
	fprintf(stderr, type);
	fprintf(stderr, ": ");
	fprintf(stderr, message);
	fprintf(stderr, "\n");
}

#endif
