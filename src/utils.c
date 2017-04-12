#include "utils.h"

bool is_parent_view(wlc_handle p, wlc_handle view) {
	wlc_handle parent = wlc_view_get_parent(view);
	if (parent == 0)
		return false;
	return parent == p || is_parent_view(p, parent);
}
