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

void print_debug_anchor(enum wlc_positioner_anchor_bit abit) {
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

void print_debug_gravity(enum wlc_positioner_gravity_bit gravity) {
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

void print_debug_adjustment(enum wlc_positioner_constraint_adjustment_bit adj) {
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

// https://git.enlightenment.org/core/enlightenment.git/tree/src/modules/wl_desktop_shell/xdg6.c#n643

struct positioner {
	const struct wlc_size* size;
	const struct wlc_geometry* anchor;
	const struct wlc_point* offset;

	enum wlc_positioner_anchor_bit abit;
	enum wlc_positioner_gravity_bit gravity;
	enum wlc_positioner_constraint_adjustment_bit adj;
};

static bool constrained(struct wlc_geometry* pos, struct wlc_geometry* parent) {
	return !wlc_geometry_contains(parent, pos);
}

static bool contains(struct wlc_geometry* parent, int x, int y, int w, int h) {
	struct wlc_geometry qg = { {x, y}, {w, h} };
	return wlc_geometry_contains(parent, &qg);
}

static int apply_positioner_x(int x, struct positioner* p, bool invert) {
	enum wlc_positioner_anchor_bit anch = WLC_BIT_ANCHOR_NONE;
	enum wlc_positioner_gravity_bit grav = WLC_BIT_GRAVITY_NONE;

	if (!invert) {
		anch = p->abit;
		grav = p->gravity;
	} else {
		if (p->abit & WLC_BIT_ANCHOR_LEFT)
			anch |= WLC_BIT_ANCHOR_RIGHT;
		else if (p->abit & WLC_BIT_ANCHOR_RIGHT)
			anch |= WLC_BIT_ANCHOR_LEFT;

		if (p->gravity & WLC_BIT_GRAVITY_LEFT)
			grav |= WLC_BIT_GRAVITY_RIGHT;
		else if (p->gravity & WLC_BIT_GRAVITY_RIGHT)
			grav |= WLC_BIT_GRAVITY_LEFT;
	}

	if (anch & WLC_BIT_ANCHOR_LEFT)
		// left edge
		x += p->anchor->size.w;
	else if (anch & WLC_BIT_ANCHOR_RIGHT)
		// right edge
		x += p->anchor->origin.x + p->anchor->size.w;
	else
		// center
		x += p->anchor->origin.x + (p->anchor->size.w / 2);

	if (grav & WLC_BIT_GRAVITY_LEFT)
		// flit left over anchor
		x -= p->size->w;
	else if (!(grav & WLC_BIT_GRAVITY_RIGHT))
		// center on anchor
		x -= p->size->w / 2;

	return x;
}

static int apply_positioner_y(int y, struct positioner* p, bool invert) {
	enum wlc_positioner_anchor_bit anch = WLC_BIT_ANCHOR_NONE;
	enum wlc_positioner_gravity_bit grav = WLC_BIT_GRAVITY_NONE;

	if (!invert) {
		anch = p->abit;
		grav = p->gravity;
	} else {
		if (p->abit & WLC_BIT_ANCHOR_TOP)
			anch |= WLC_BIT_ANCHOR_BOTTOM;
		else if (p->abit & WLC_BIT_ANCHOR_BOTTOM)
			anch |= WLC_BIT_ANCHOR_TOP;

		if (p->gravity & WLC_BIT_GRAVITY_TOP)
			grav |= WLC_BIT_GRAVITY_BOTTOM;
		else if (p->gravity & WLC_BIT_GRAVITY_BOTTOM)
			grav |= WLC_BIT_GRAVITY_TOP;
	}

	if (anch & WLC_BIT_ANCHOR_TOP)
		// top edge
		y += p->anchor->size.h;
	else if (anch & WLC_BIT_ANCHOR_BOTTOM)
		// bottom edge
		y += p->anchor->origin.y + p->anchor->size.h;
	else
		// center
		y += p->anchor->origin.y + (p->anchor->size.h / 2);

	if (grav & WLC_BIT_GRAVITY_TOP)
		// flit top over anchor
		y -= p->size->h;
	else if (!(grav & WLC_BIT_GRAVITY_BOTTOM))
		// center on anchor
		y -= p->size->h / 2;

	return y;
}

static bool apply_positioner_slide(struct wlc_geometry* result, struct wlc_geometry* parent,
		struct positioner* p) {
	if ((p->adj && WLC_BIT_CONSTRAINT_ADJUSTMENT_SLIDE_X) &&
		(!contains(parent, result->origin.x, parent->origin.y, result->size.w, 1))) {
		int sx = result->origin.x;

		if (p->gravity & WLC_BIT_GRAVITY_LEFT) {
			if (result->origin.x + result->size.w > parent->origin.x + parent->size.w)
				sx = MAX(parent->origin.x + parent->size.w - result->size.w,
						parent->origin.x + p->anchor->origin.x - result->size.w);
			else if (result->origin.x < parent->origin.x)
				sx = MIN(parent->origin.x, parent->origin.x + p->anchor->origin.x + p->anchor->size.w);
		} else if (p->gravity & WLC_BIT_GRAVITY_RIGHT) {
			if (result->origin.x < parent->origin.x)
				sx = MIN(parent->origin.x, parent->origin.x + p->anchor->origin.x + p->anchor->size.w);
			else if (result->origin.x + result->size.w > parent->origin.x + parent->size.w)
				sx = MAX(parent->origin.x + parent->size.w - result->size.w,
						parent->origin.x + p->anchor->origin.x - result->size.w);
		}

		if (contains(parent, sx, parent->origin.y, result->size.w, 1))
			result->origin.x = sx;
	}

	if (!constrained(result, parent))
		return true; // not constrained

	if ((p->adj && WLC_BIT_CONSTRAINT_ADJUSTMENT_SLIDE_Y) &&
		(!contains(parent, parent->origin.y, result->origin.y, 1, result->size.h))) {
		int sy = result->origin.y;

		if (p->gravity & WLC_BIT_GRAVITY_TOP) {
			if (result->origin.y + result->size.h > parent->origin.y + parent->size.h)
				sy = MAX(parent->origin.y + parent->size.h - result->size.h,
						parent->origin.y + p->anchor->origin.y - result->size.h);
			else if (result->origin.y < parent->origin.y)
				sy = MIN(parent->origin.y, parent->origin.y + p->anchor->origin.y + p->anchor->size.h);
		} else if (p->gravity & WLC_BIT_GRAVITY_BOTTOM) {
			if (result->origin.y < parent->origin.y)
				sy = MIN(parent->origin.y, parent->origin.y + p->anchor->origin.y + p->anchor->size.h);
			else if (result->origin.y + result->size.h > parent->origin.y + parent->size.h)
				sy = MAX(parent->origin.y + parent->size.h - result->size.h,
						parent->origin.y + p->anchor->origin.y - result->size.h);
		}

		if (contains(parent, parent->origin.x, sy, 1, result->size.h))
			result->origin.y = sy;
	}

	return false;
}

static void apply_positioner(struct wlc_geometry* result, struct wlc_geometry* parent,
		struct positioner* p) {

	struct wlc_geometry copypos = *result;

	result->origin.x = parent->origin.x + p->offset->x;
	result->origin.y = parent->origin.y + p->offset->y;

	if (p->size && p->size->w && p->size->h) {
		// honor the positioner size request
		result->size.w = p->size->w;
		result->size.h = p->size->h;
	}

	// apply anchor then gravity
	result->origin.x = apply_positioner_x(result->origin.x, p, false);
	result->origin.y = apply_positioner_y(result->origin.y, p, false);

	if (!constrained(result, parent))
		return; // fits, return

	// apply flip, slide
	if ((p->adj & WLC_BIT_CONSTRAINT_ADJUSTMENT_FLIP_X) &&
			(!contains(parent, result->origin.x, parent->origin.y, result->size.w, 1))) {
		// flip on x
		int fx = apply_positioner_x(result->origin.x, p, true);
		if (contains(parent, fx, parent->origin.y, result->size.w, 1))
			result->origin.x = fx;
	}

	if (!constrained(result, parent))
		return; // fits, return

	if ((p->adj & WLC_BIT_CONSTRAINT_ADJUSTMENT_FLIP_Y) &&
			(!contains(parent, parent->origin.x, result->origin.y, 1, result->size.h))) {
		// flip on y
		int fy = apply_positioner_y(result->origin.y, p, true);

		if (contains(parent, parent->origin.x, fy, 1, result->size.h))
			result->origin.y = fy;
	}

	if (!constrained(result, parent))
		return; // fits, return

	if (apply_positioner_slide(result, parent, p))
		return; // fits

	apply_positioner_slide(result, &copypos, p);
	// resize is not applicable
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
	struct wlc_geometry* pgeometry = (struct wlc_geometry*)wlc_view_get_geometry(parent);

#if (0) // debug info
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
#endif

	struct positioner p;
	p.size = size_req;
	p.anchor = anchor;
	p.offset = offset;
	p.abit = abit;
	p.gravity = gravity;
	p.adj = adj;

	apply_positioner(result, pgeometry, &p);
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
