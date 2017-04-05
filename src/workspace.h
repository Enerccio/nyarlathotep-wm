/*
 * Workspace subsystem.
 */
#ifndef WORKSPACE_H_
#define WORKSPACE_H_

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

	wlc_handle fullscreen_view;
	wlc_handle main_view;

	list_t* hidden_windows;

	bool display_pinion_button; // TODO
} workspace_t;

workspace_t* create_workspace(wlc_handle handle);
void terminate_workspace(workspace_t* workspace);

#endif /* WORKSPACE_H_ */
