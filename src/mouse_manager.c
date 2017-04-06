#include "pinion.h"

bool mouse_motion(wlc_handle view, uint32_t time,
		const struct wlc_point* pointer) {

	wlc_pointer_set_position(pointer);
	return false;
}
