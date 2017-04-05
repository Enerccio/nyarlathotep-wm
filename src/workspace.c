#include "libds/hmap.h"
#include "pinion.h"
#include "workspace.h"

/* Holds output->workspace hash map */
hash_table_t* workspaces;

/**
 * Initializes workspace subsystem.
 */
void init_workspaces() {
	workspaces = create_uint64_table();
}

/**
 * Called when output is created by wlc.
 *
 * Handles creation of workspace for each output.
 */
bool output_created(wlc_handle output) {
	workspace_t* workspace = create_workspace(output);
	if (workspace == NULL)
		return false; // failed to allocate memory
	table_set(workspaces, (void*)output, (void*)workspace);
	return true;
}

/**
 * Called when output terminates.
 *
 * Workspace for that output is terminated.
 */
void output_terminated(wlc_handle output) {
	workspace_t* workspace = table_get(workspaces, (void*)output);
	if (workspace != NULL) {
		terminate_workspace(workspace);
		table_remove(workspaces, (void*)output); // clear the
												 //  association from table
	}
}

/**
 * creates and initializes workspace
 *
 * returns NULL on failure (memory insufficient)
 */
workspace_t* create_workspace(wlc_handle output) {
	workspace_t* workspace = malloc(sizeof(workspace_t));
	if (workspace == NULL)
		return NULL;

	memset(workspace, 0, sizeof(workspace_t));

	workspace->output = output;
	workspace->hidden_windows = create_list();

	if (workspace->hidden_windows == NULL) {
		free(workspace);
		return NULL;
	}

	return workspace;
}

/**
 * terminates workspace
 *
 * frees the underlying memory structures (deep clean)
 */
void terminate_workspace(workspace_t* workspace) {
	// TODO
	free(workspace);
}
