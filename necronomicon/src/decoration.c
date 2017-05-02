#include "necronomicon.h"

#include "libds/hmap.h"

void destroy_decoration(struct decoration* decoration) {

}

struct decoration* create_decoration(uint32_t handle, enum decorator_decoratee_state state, const char* title) {
	struct decoration* decoration = malloc(sizeof(struct decoration));
	if (decoration == NULL)
		ERROR_MEM("failed to allocate decoration");
	memset(decoration, 0, sizeof(struct decoration));

	decoration->width = 1;
	decoration->height = 1;
	decoration->handle = handle;
	decoration->title = title;
	decoration->state = state;

	decoration->surface = wl_compositor_create_surface(compositor);
	if (decoration->surface == NULL) {
		ERROR("failed to create surface for decoration");
	}

	decoration->shell_surface = wl_shell_get_shell_surface(shell, decoration->surface);
	if (decoration->shell_surface == NULL) {
		ERROR("failed to create shell surface");
	}

	init_decoration(decoration);
	wl_shell_surface_set_toplevel(decoration->shell_surface);

	draw_decoration(decoration);

	return decoration;
}
