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
	}
}
 */

list_t* launchers;

static void clear_launcher(launcher_t* launcher) {
	if (launcher) {
		free(launcher->file);
		free(launcher->name);
		free(launcher->display_name);
		free(launcher->launcher.program);
		for (int i=0; i<launcher->launcher.arguments.argc; i++) {
			free(launcher->launcher.arguments.argv[i]);
		}
		free(launcher->launcher.arguments.argv);
		for (int i=0; i<launcher->categories.count; i++) {
			free(launcher->categories.names[i]);
		}
		free(launcher->categories.names);
	}
	free(launcher);
}

#define FAIL_LAUNCHER(fname, reason) do { \
		char buf[2048]; \
		sprintf(buf, "failed to load launcher file %s: %s", fname, reason);\
		LOG_ERROR(buf); \
	} while (0)

static launcher_t* create_launcher(const char* launcher_file) {
	launcher_t* l = malloc(sizeof(launcher_t));
	ASSERT_MEM(l);
	memset(l, 0, sizeof(launcher_t));

	l->file = copy_string(launcher_file);

	aojls_deserialization_prefs prefs;
	memset(&prefs, 0, sizeof(aojls_deserialization_prefs));
	size_t maxl;
	char* fdata = read_file_to_string(launcher_file, &maxl);

	if (!fdata) {
		clear_launcher(l);
		FAIL_LAUNCHER(launcher_file, "disk IO error");
		return NULL;
	}

	aojls_ctx_t* context = aojls_deserialize(fdata, maxl, &prefs);
	ASSERT_MEM(context);

	if (json_context_error_happened(context))
		goto jsonfailure;

	json_value_t* result = json_context_get_result(context);

	if (json_get_type(result) != JS_OBJECT)
		goto jsonfailure;

	json_object* root = json_as_object(result);

	l->name = copy_string(json_object_get_string_default(root, "name", ""));
	l->display_name = copy_string(json_object_get_string_default(root, "display-name", ""));

	json_object* keys = json_object_get_object(root, "keys");
	if (keys) {
		bool valid1, valid2;

		l->launch_configuration.key = (uint32_t)json_object_get_double(keys, "key", &valid1);
		l->launch_configuration.mods = (long)json_object_get_double(keys, "mods", &valid2);

		l->launch_configuration.has_launch_config = valid1 && valid2;
	}

	json_object* command = json_object_get_object(root, "launcher");
	if (command) {
		l->launcher.program = copy_string(json_object_get_string_default(command, "program", ""));
		json_array* args = json_object_get_array(command, "arguments");

		l->launcher.arguments.argc = 1 + (args ? json_array_size(args) : 0);
		l->launcher.arguments.argv = malloc(sizeof(char*) * (1 + l->launcher.arguments.argc));
		ASSERT_MEM(l->launcher.arguments.argv);

		l->launcher.arguments.argv[0] = copy_string(l->launcher.program);

		for (int i=0; i<l->launcher.arguments.argc-1; i++) {
			l->launcher.arguments.argv[i+1] = copy_string(json_array_get_string_default(args, i, ""));
		}

		l->launcher.arguments.argv[l->launcher.arguments.argc] = NULL;
	}

	json_array* categories = json_object_get_array(root, "categories");
	if (categories) {
		l->categories.count = json_array_size(categories);
		l->categories.names = malloc(sizeof(char*) * l->categories.count);
		ASSERT_MEM(l->categories.names);

		for (int i=0; i<l->categories.count; i++) {
			l->categories.names[i] = copy_string(json_array_get_string_default(categories, i, ""));
		}
	}

	json_free_context(context);
	return l;

jsonfailure:
	clear_launcher(l);
	json_free_context(context);
	FAIL_LAUNCHER(launcher_file, "json parse failure");
	return NULL;
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
				clear_launcher(launcher);
			}
		}

		if (event->type == DELETED) {
			unregister_notifier(launcher_file);
		}
	}

	if (event->type == CREATED || event->type == MODIFIED) {
		launcher_t* launcher = create_launcher(launcher_file);
		if (launcher != NULL) {
			list_push_right(launchers, launcher);

			if (event->type == CREATED) {
				register_notifier(copy_string(launcher_file), launchers_modified, NULL);
			}
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
				if (launcher != NULL) {
					list_push_right(launchers, launcher);
					register_notifier(copy_string(fp), launchers_modified, NULL);
				}
			}
		}

		free(fp);
		free(entry);
	}
	free_list(dirent);
}

bool launch_launcher(launcher_t* launcher) {
	if (launcher && launcher->launcher.program) {
		wlc_exec((const char*)launcher->launcher.program, (char* const*)launcher->launcher.arguments.argv);
	}
	return false;
}

bool launcher_execute_key_pressed(uint32_t key, struct wlc_modifiers mods) {
	list_iterator_t li;
	list_create_iterator(launchers, &li);
	uint32_t symkey = wlc_keyboard_get_keysym_for_key(key, &mods);
	while (list_has_next(&li)) {
		launcher_t* l = (launcher_t*)list_next(&li);
		if (l->launch_configuration.has_launch_config) {
			if (symkey == l->launch_configuration.key &&
					mods.mods == l->launch_configuration.mods) {
				return launch_launcher(l);
			}
		}
	}
	return false;
}

void init_launchers(const char* from) {
	launchers = create_list();
	load_init_launcher_files(from);
	register_notifier(from, launchers_modified, NULL);
}
