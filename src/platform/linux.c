#ifdef linux

#include "../configuration.h"
#include "../nyarlathotep.h"
#include "platform.h"
#include "../utils.h"
#include "../libds/hmap.h"
#include "../libds/llist.h"

#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/inotify.h>
#include <limits.h>

#ifdef journal_d
#include <syslog.h>
#endif

void inotify_register_watcher();

static hash_table_t* notifiers;
static hash_table_t* watchers;
static int notify_fd;

void init_platform() {
	notifiers = create_string_table();
	watchers = create_uint64_table();

	notify_fd = inotify_init();
	ASSERT((!(notify_fd == -1)), "failed to initialize inotify");
}

void init_platform_compositor() {
	inotify_register_watcher();
}

char* path_concat(const char* path, const char* element) {
	if (path == NULL)
		return NULL;

	size_t sp = strlen(path);
	size_t se = 0;
	if (element)
		se = strlen(element);
	char* np = malloc(sizeof(char) * (sp + se + 2));

	ASSERT_MEM(np);

	memcpy(np, path, sp);
	if (se == 0) {
		np[sp] = '\0';
		return np;
	}

	np[sp] = '/';
	memcpy(np + (sp + 1), element, se);
	np[sp + se + 1] = '\0';

	return np;
}

bool path_is_absolute(const char* path) {
	if (path == NULL)
		return false;
	if (strlen(path) > 0 && path[0] == '/')
		return true;
	return false;
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

void get_directory_content(const char* directory, list_t* result) {
	if (directory_exists(directory)) {
		DIR* dir = opendir(directory);
		struct dirent* entry;
		while ((entry = readdir(dir))) {
			list_push_right(result, copy_string(entry->d_name));
		}
	}
}

char* get_nyarlathotep_workdir() {
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

struct notify_holder {
	int watcher;
	list_t* callbacks;
};

struct notify_callback {
	void(*callback)(const struct notify_event* event);
	void* data;
};

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
static char inotify_read_buffer[BUF_LEN];

static void handle_inotify_event(struct inotify_event* i) {
	if (table_contains(watchers, (void*)(uint64_t)i->wd)) {
		const char* path = (const char*)table_get(watchers, (void*)(uint64_t)i->wd);
		struct notify_holder* holder = (struct notify_holder*)table_get(notifiers, (void*)path);

		struct notify_event event;

		event.register_path = copy_string(path);
		event.element = i->len == 0 ? "" : copy_string(i->name);

		if (i->mask & IN_CREATE) {
			event.type = CREATED;
		} else if (i->mask & IN_MODIFY) {
			event.type = MODIFIED;
		} else {
			event.type = DELETED;
		}

		event.isdir = i->mask & IN_ISDIR;

		list_iterator_t it;
		list_create_iterator(holder->callbacks, &it);
		while (list_has_next(&it)) {
			struct notify_callback* cb = (struct notify_callback*)list_next(&it);
			event.data = cb->data;
			cb->callback(&event);
		}

		free(event.register_path);
		free(i->len == 0 ? 0 : event.element);
	}
}

static int notify_read(int fd, uint32_t mask, void* data) {
	int rl = read(fd, inotify_read_buffer, BUF_LEN);
	if (rl) {
		if (rl == -1)
			GENERAL_FAILURE("inotify failed to read");
		for (char* p = inotify_read_buffer; p < inotify_read_buffer + rl; ) {
			struct inotify_event* event = (struct inotify_event *) p;
			handle_inotify_event(event);
			p += sizeof(struct inotify_event) + event->len;
		}
	}
	return 0;
}

void inotify_register_watcher() {
	wlc_event_loop_add_fd(notify_fd, WLC_EVENT_READABLE, notify_read, NULL);
}

static int init_watcher(const char* path) {
	return inotify_add_watch(notify_fd, path, IN_CREATE | IN_MODIFY | IN_DELETE );
}

void register_notifier(const char* path, void(callback)(const struct notify_event* event), void* data) {
	if (!table_contains(notifiers, (void*)path)) {
		struct notify_holder* nh = malloc(sizeof(struct notify_holder));
		ASSERT_MEM(nh);
		nh->callbacks = create_list();
		ASSERT_MEM(nh->callbacks);

		nh->watcher = init_watcher(path);
		if (nh->watcher == -1) {
			free_list(nh->callbacks);
			free(nh);
			return;
		}

		table_set(watchers, (void*)(uint64_t)nh->watcher, (void*)path);
		table_set(notifiers, (void*)path, nh);
	}

	struct notify_holder* holder = (struct notify_holder*)table_get(notifiers, (void*)path);
	struct notify_callback* cb = malloc(sizeof(struct notify_callback));
	ASSERT_MEM(cb);
	cb->data = data;
	cb->callback = callback;

	list_push_right(holder->callbacks, cb);
}

void unregister_notifier(const char* path) {
	if (!table_contains(notifiers, (void*)path))
		return;
	struct notify_holder* holder = (struct notify_holder*)table_get(notifiers, (void*)path);
	int wd = holder->watcher;
	inotify_rm_watch(notify_fd, wd);
	list_iterator_t it;
	list_create_iterator(holder->callbacks, &it);
	while (list_has_next(&it)) {
		free(list_next(&it));
	}
	free_list(holder->callbacks);
	free(holder);
	table_remove(notifiers, (void*)path);
	table_remove(watchers, (void*)(uint64_t)wd);
}

void unregister_notifier_callback(const char* path, void(callback)(struct notify_event event)) {
	if (!table_contains(notifiers, (void*)path))
		return;
	struct notify_holder* holder = (struct notify_holder*)table_get(notifiers, (void*)path);
	list_iterator_t it;
	list_create_iterator(holder->callbacks, &it);
	while (list_has_next(&it)) {
		struct notify_callback* cb = (struct notify_callback*)list_next(&it);
		if ((void*)cb->callback == (void*)callback) {
			free(cb->data);
			free(cb);
			list_remove_it(&it);
		}
	}
}

void unregister_notifier_callback_data(const char* path, void(callback)(struct notify_event event), void* data) {
	if (!table_contains(notifiers, (void*)path))
			return;
	struct notify_holder* holder = (struct notify_holder*)table_get(notifiers, (void*)path);
	list_iterator_t it;
	list_create_iterator(holder->callbacks, &it);
	while (list_has_next(&it)) {
		struct notify_callback* cb = (struct notify_callback*)list_next(&it);
		if ((void*)cb->callback == (void*)callback && data == cb->data) {
			free(cb->data);
			free(cb);
			list_remove_it(&it);
		}
	}
}

#endif
