#include "pinion.h"

bool view_created(wlc_handle view) {
	workspace_t* workspace = get_workspace_for_view(view);
	if (workspace == NULL)
		return false;
	workspace_set_main_window(workspace, view);
	return true;
}
