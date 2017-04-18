#include "nyarlathotep.h"

bool view_created(wlc_handle view) {
	LOG_DEBUG("view created");

	workspace_t* workspace = get_workspace_for_view(view);
	if (workspace == NULL)
		return false;
	workspace_set_main_window(workspace, view);
	return true;
}

void view_focus_change(wlc_handle view, bool focus) {
	LOG_DEBUG("view focus changed");

	workspace_t* workspace = get_workspace_for_view(view);
	if (workspace == NULL)
		return;

	if (focus) {
		workspace_view_got_focus(workspace, view);
	} else {
		wlc_view_set_state(view, WLC_BIT_ACTIVATED, false);
	}
}

void view_destroyed(wlc_handle view) {
	LOG_DEBUG("view destroyed");

	workspace_t* workspace = get_workspace_for_view(view);
	if (workspace == NULL)
		return;
	workspace_view_destroyed(workspace, view);
}

void state_change(wlc_handle view, enum wlc_view_state_bit state, bool change) {
	LOG_DEBUG("view state changed");

	workspace_t* workspace = get_workspace_for_view(view);
	if (workspace == NULL)
		return;
	if (state & WLC_BIT_MAXIMIZED) {
		workspace_maximize_request(workspace, view, change);
	}
}

static void print_debug_anchor(enum wlc_positioner_anchor_bit abit) {
	if (abit == WLC_BIT_ANCHOR_NONE) {
		printf("NONE ");
	} else {
		if (abit & WLC_BIT_ANCHOR_TOP) {
			printf("TOP ");
		}
		if (abit & WLC_BIT_ANCHOR_BOTTOM) {
			printf("BOTTOM ");
		}
		if (abit & WLC_BIT_ANCHOR_LEFT) {
			printf("LEFT ");
		}
		if (abit & WLC_BIT_ANCHOR_RIGHT) {
			printf("RIGHT ");
		}
	}

	printf("\n");
}

static void print_debug_gravity(enum wlc_positioner_gravity_bit gravity) {
	if (gravity == WLC_BIT_GRAVITY_NONE) {
		printf("NONE ");
	} else {
		if (gravity & WLC_BIT_GRAVITY_TOP) {
			printf("TOP ");
		}
		if (gravity & WLC_BIT_GRAVITY_BOTTOM) {
			printf("BOTTOM ");
		}
		if (gravity & WLC_BIT_GRAVITY_LEFT) {
			printf("LEFT ");
		}
		if (gravity & WLC_BIT_GRAVITY_RIGHT) {
			printf("RIGHT ");
		}
	}
	printf("\n");
}

static void print_debug_adjustment(enum wlc_positioner_constraint_adjustment_bit adj) {
	if (adj == WLC_BIT_CONSTRAINT_ADJUSTMENT_NONE) {
		printf("NONE ");
	} else {
		if (adj & WLC_BIT_CONSTRAINT_ADJUSTMENT_SLIDE_X) {
			printf("SLIDE X ");
		}
		if (adj & WLC_BIT_CONSTRAINT_ADJUSTMENT_SLIDE_Y) {
			printf("SLIDE Y ");
		}
		if (adj & WLC_BIT_CONSTRAINT_ADJUSTMENT_FLIP_X) {
			printf("FLIP X ");
		}
		if (adj & WLC_BIT_CONSTRAINT_ADJUSTMENT_FLIP_Y) {
			printf("FLIP Y ");
		}
		if (adj & WLC_BIT_CONSTRAINT_ADJUSTMENT_RESIZE_X) {
			printf("RESIZE X ");
		}
		if (adj & WLC_BIT_CONSTRAINT_ADJUSTMENT_RESIZE_Y) {
			printf("RESIZE Y ");
		}
	}
	printf("\n");
}

// this madness: https://lists.freedesktop.org/archives/wayland-devel/2016-May/029076.html
static void set_positioner_geometry(wlc_handle view, struct wlc_geometry* result,
		const struct wlc_geometry* original_g_request, workspace_t* workspace) {
	const struct wlc_size* size_req = wlc_view_positioner_get_size(view);
	const struct wlc_geometry* anchor = wlc_view_positioner_get_anchor_rect(view);
	const struct wlc_point* offset = wlc_view_positioner_get_offset(view);
	enum wlc_positioner_anchor_bit abit = wlc_view_positioner_get_anchor(view);
	enum wlc_positioner_gravity_bit gravity = wlc_view_positioner_get_gravity(view);
	enum wlc_positioner_constraint_adjustment_bit adj = wlc_view_positioner_get_constraint_adjustment(view);
	wlc_handle parent = wlc_view_get_parent(view);

	printf("\n");
	printf("positioner info for view %d\n", (int)view);
	if (anchor)
		printf("positioner anchor {x: %d, y: %d, w: %d, h: %d}\n",
				anchor->origin.x, anchor->origin.y, anchor->size.w, anchor->size.h);
	if (offset)
		printf("positioner offset {x: %d, y: %d}\n", offset->x, offset->y);
	if (size_req)
		printf("positioner size_req {x: %d, y: %d}\n", size_req->w, size_req->h);
	printf("positioner anchor "); print_debug_anchor(abit);
	printf("positioner gravity "); print_debug_gravity(gravity);
	printf("positioner adjustment "); print_debug_adjustment(adj);
	printf("\n");

	fflush(stdout);

	if (size_req && (size_req->w > 0 && size_req->h > 0)) {
		result->size = *size_req; // honor the size request, but it should be the same
	}

	// TODO

}

void request_geometry(wlc_handle view, const struct wlc_geometry* geometry) {
	workspace_t* workspace = get_workspace_for_view(view);
	if (workspace == NULL)
		return;

	wlc_handle parent = wlc_view_get_parent(view);
	if (parent == 0) {
		// no effect to prevent user resizes
	} else {
		struct wlc_geometry g;
		g = *geometry;

		set_positioner_geometry(view, &g, geometry, workspace);
		handle_float_view_geometry(workspace, view, &g);
	}
}

bool minimize_request(wlc_handle view, bool minimize) {
	workspace_t* workspace = get_workspace_for_view(view);
	if (workspace == NULL)
		return true;
	return workspace_minimize_request(workspace, view, minimize);
}
