#include "libds/hmap.h"
#include "pinion.h"
#include "workspace.h"

hash_table_t* workspaces;

void init_workspaces() {
	workspaces = create_uint64_table();
}

bool output_created(wlc_handle output) {
	workspace_t* workspace = create_workspace(output);
	if (workspace == NULL)
		return false; // failed to allocate memory
	table_set(workspaces, (void*)output, (void*)workspace);
	return true;
}

void output_terminated(wlc_handle output) {
	workspace_t* workspace = table_get(workspaces, (void*)output);
	if (workspace != NULL)
		terminate_workspace(workspace);
}

workspace_t* create_workspace(wlc_handle output) {
	workspace_t* workspace = malloc(sizeof(workspace_t));
	if (workspace == NULL)
		return NULL;

	workspace->output = output;

	return workspace;
}

void terminate_workspace(workspace_t* workspace) {
	// TODO
	free(workspace);
}
