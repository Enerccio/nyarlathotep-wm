/*
 * Workspace subsystem.
 */
#ifndef WORKSPACE_H_
#define WORKSPACE_H_

#include <GLES2/gl2.h>
#include <wlc/wlc.h>

#include "libds/llist.h"

/**
 * Workspace structure.
 *
 * Every output has one workspace structure assigned.
 * Workspace represents single displayed main window and multiple
 * pinned windows. Also, it contains list of hidden windows.
 *
 * First created workspace will also contain pinion button.
 */
typedef struct workspace {
	wlc_handle output;
	int w, h;
	int px, py;

	wlc_handle fullscreen_view;
	wlc_handle main_view;

	list_t* hidden_windows;
	GLuint square_shader;
	GLuint square_color_shader;

	bool window_list_opening;
	bool window_list_force_hide;
	int window_list_total_width;
	int window_list_show_width;
	int window_list_scroll_offset;

	GLuint background_texture;

	bool display_pinion_button; // TODO
} workspace_t;

workspace_t* create_workspace(wlc_handle handle);
void terminate_workspace(workspace_t* workspace);
void resize_workspace(workspace_t* workspace);
void open_window_list(workspace_t* workspace);
void close_window_list(workspace_t* workspace);

#endif /* WORKSPACE_H_ */
