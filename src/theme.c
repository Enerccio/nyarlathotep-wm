#include "theme.h"

theme_t* active_theme = NULL;

struct rgb_color sentinel_value_lc = {1, 1, 1};
struct rgb_color sentinel_value_lsc = {1, 0.4f, 1};

#define FAIL_THEME(fname, reason) do { \
		char buf[2048]; \
		sprintf(buf, "failed to load theme file %s: %s", fname, reason);\
		LOG_ERROR(buf); \
	} while (0)

void set_active_theme(const char* file) {
	theme_t* theme = (theme_t*)malloc(sizeof(theme_t));
	ASSERT_MEM(theme);

	if (active_theme != NULL) {
		free(active_theme);
		active_theme = NULL;
	}

	aojls_deserialization_prefs prefs;
	memset(&prefs, 0, sizeof(aojls_deserialization_prefs));
	size_t maxl;
	char* fdata = read_file_to_string(file, &maxl);

	if (!fdata) {
		free(theme);
		FAIL_THEME(file, "disk IO error");
		return;
	}

	aojls_ctx_t* context = aojls_deserialize(fdata, maxl, &prefs);
	ASSERT_MEM(context);

	if (json_context_error_happened(context))
		goto jsonfailure;

	json_value_t* result = json_context_get_result(context);

	if (json_get_type(result) != JS_OBJECT)
		goto jsonfailure;

	json_object* root = json_as_object(result);

	json_object* thumbnails = json_object_get_object(root, "thumbnails");
	if (thumbnails) {
		theme->window_list_sizes.height = (int)json_object_get_double_default(thumbnails, "height", 100);
		theme->window_list_sizes.offset = (int)json_object_get_double_default(thumbnails, "offset", 130);
	} else {
		theme->window_list_sizes.height = 100;
		theme->window_list_sizes.offset = 130;
	}

	json_object* list = json_object_get_object(root, "list");
	if (list != NULL) {
		bool valid1, valid2, valid3;

		json_array* color = json_object_get_array(list, "color");
		if (color != NULL) {
			float r = (float)json_array_get_double(color, 0, &valid1);
			float g = (float)json_array_get_double(color, 1, &valid2);
			float b = (float)json_array_get_double(color, 2, &valid3);
			if (valid1 && valid2 && valid3) {
				theme->window_list_color.r = r;
				theme->window_list_color.g = g;
				theme->window_list_color.b = b;
			} else {
				theme->window_list_color = sentinel_value_lc;
			}
		} else {
			theme->window_list_color = sentinel_value_lc;
		}

		json_array* selection = json_object_get_array(list, "selection");
		if (selection != NULL) {
			float r = (float)json_array_get_double(selection, 0, &valid1);
			float g = (float)json_array_get_double(selection, 1, &valid2);
			float b = (float)json_array_get_double(selection, 2, &valid3);
			if (valid1 && valid2 && valid3) {
				theme->window_selection_color.r = r;
				theme->window_selection_color.g = g;
				theme->window_selection_color.b = b;
			} else {
				theme->window_selection_color = sentinel_value_lsc;
			}
		} else {
			theme->window_selection_color = sentinel_value_lsc;
		}
	} else {
		theme->window_list_color = sentinel_value_lc;
		theme->window_selection_color = sentinel_value_lsc;
	}

	active_theme = theme;
	json_free_context(context);
	return;
jsonfailure:
	free(theme);
	json_free_context(context);
	FAIL_THEME(file, "json parse failure");
	return;
}

struct rgb_color get_window_list_color() {
	if (active_theme == NULL)
		return sentinel_value_lc;
	return active_theme->window_list_color;
}

struct rgb_color get_window_list_selection_color() {
	if (active_theme == NULL)
		return sentinel_value_lsc;
	return active_theme->window_selection_color;
}

int get_side_window_height() {
	if (active_theme == NULL)
		return 100;
	return active_theme->window_list_sizes.height;
}

int get_size_window_offset() {
	if (active_theme == NULL)
		return 130;
	return active_theme->window_list_sizes.offset;
}
