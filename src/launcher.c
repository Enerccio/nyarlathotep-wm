#include "launcher.h"

/*
 {
	"name": "gnome-editor",
	"caregory" : ["Editors"],
	"display-name": "GEdit",
	"launcher": {
		"program": "/usr/bin/gedit",
		"arguments": []
	},
	"keys": {
		"mods": 12,
		"key": 101
	},
	"display": {
		"hidden": true
	}
 }
 */

list_t* launchers;

static launcher_t* create_launcher(const char* launcher_file) {
	launcher_t* l = malloc(sizeof(launcher_t));
	ASSERT_MEM(l);
	l->file = copy_string(launcher_file);

	aojls_deserialization_prefs prefs;
	memset(&prefs, 0, sizeof(aojls_deserialization_prefs));
	size_t maxl;
	char* fdata = read_file_to_string(launcher_file, &maxl);
	if (fdata) {
		aojls_ctx_t* context = aojls_deserialize(fdata, maxl, &prefs);
		if (!json_context_error_happened(context)) {

		}
	}

	return l;
}

void launchers_modified(const struct notify_event* event) {
	char* launcher_file = path_concat(event->register_path, event->element);

	if (event->isdir)
		return;

	char* dot = strrchr(launcher_file, '.');
	if (!dot || strcmp(dot, ".nya")) {
		free(launcher_file);
		return;
	}

	if (event->type == DELETED || event->type == MODIFIED) {
		list_iterator_t it;
		for (list_create_iterator(launchers, &it); list_has_next(&it); ) {
			launcher_t* launcher = (launcher_t*)list_next(&it);
			if (strcmp(launcher->file, launcher_file)==0) {
				list_remove_it(&it);
				free(launcher);
			}
		}

		if (event->type == DELETED) {
			unregister_notifier(launcher_file);
		}
	}

	if (event->type == CREATED || event->type == MODIFIED) {
		launcher_t* launcher = create_launcher(launcher_file);
		list_push_right(launchers, launcher);

		if (event->type == CREATED) {
			register_notifier(copy_string(launcher_file), launchers_modified, NULL);
		}
	}

	free(launcher_file);
}

static void load_init_launcher_files(const char* from) {
	list_t* dirent = create_list();
	get_directory_content(from, dirent);

	list_iterator_t li;
	for (list_create_iterator(dirent, &li); list_has_next(&li); ) {
		char* entry = (char*)list_next(&li);

		char* fp = path_concat(from, entry);
		if (!directory_exists(entry)) {
			// is not directory
			char* dot = strrchr(fp, '.');
			if (dot && strcmp(dot, ".nya") == 0) {
				launcher_t* launcher = create_launcher(fp);
				list_push_right(launchers, launcher);
				register_notifier(copy_string(fp), launchers_modified, NULL);
			}
		}

		free(fp);
		free(entry);
	}
	free_list(dirent);
}

void init_launchers(const char* from) {
	launchers = create_list();
	load_init_launcher_files(from);
	register_notifier(from, launchers_modified, NULL);
}
