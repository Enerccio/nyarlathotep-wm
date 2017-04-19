#include <wlc/wlc.h>
#include <wlc/wlc-wayland.h>
#include "../macros.h"

#include "../../generated/headers/desktop-shell.h"
#include "../../generated/includes/desktop-shell.inc"

static void set_background(struct wl_client *client,
	       struct wl_resource *resource,
	       struct wl_resource *output,
	       struct wl_resource *surface) {
	LOG_DEBUG("empty");
}

static void set_panel(struct wl_client *client,
		  struct wl_resource *resource,
		  struct wl_resource *output,
		  struct wl_resource *surface) {
	LOG_DEBUG("empty");
}

static void set_lock_surface(struct wl_client *client,
		 struct wl_resource *resource,
		 struct wl_resource *surface) {
	LOG_DEBUG("empty");
}

static void unlock(struct wl_client *client,
	       struct wl_resource *resource) {
	LOG_DEBUG("empty");
}

static void set_grab_surface(struct wl_client *client,
		 struct wl_resource *resource,
		 struct wl_resource *surface) {
	LOG_DEBUG("empty");
}

static void desktop_ready(struct wl_client *client,
	      struct wl_resource *resource) {
	LOG_DEBUG("empty");
}

static void set_panel_position(struct wl_client *client,
		   struct wl_resource *resource,
		   uint32_t position) {
	LOG_DEBUG("empty");
}

static struct desktop_shell_interface desktop_shell_interface_implementation = {
	.set_background = set_background,
	.set_panel = set_panel,
	.set_lock_surface = set_lock_surface,
	.unlock = unlock,
	.set_grab_surface = set_grab_surface,
	.desktop_ready = desktop_ready,
	.set_panel_position = set_panel_position
};

static void set_surface(struct wl_client *client,
	    struct wl_resource *resource,
	    struct wl_resource *surface,
	    struct wl_resource *output) {
	LOG_DEBUG("empty");
}

static struct screensaver_interface desktop_shell_screensaver_interface_implementation = {
	.set_surface = set_surface
};

static void desktop_shell_bind(struct wl_client *client, void *data,
		unsigned int version, unsigned int id) {
	if (version > 3) {
		// Unsupported version
		return;
	}

	struct wl_resource *resource = wl_resource_create(client, &desktop_shell_interface, version, id);
	if (!resource) {
		wl_client_post_no_memory(client);
	}

	wl_resource_set_implementation(resource, &desktop_shell_interface_implementation, NULL, NULL);
}

static void screensaver_bind(struct wl_client *client, void *data,
		unsigned int version, unsigned int id) {
	if (version > 1) {
		// Unsupported version
		return;
	}

	struct wl_resource *resource = wl_resource_create(client, &screensaver_interface, version, id);
	if (!resource) {
		wl_client_post_no_memory(client);
	}

	wl_resource_set_implementation(resource, &desktop_shell_screensaver_interface_implementation, NULL, NULL);
}

void register_desktop_shell_extension() {
	wl_global_create(wlc_get_wl_display(), &desktop_shell_interface, 3, NULL,
			desktop_shell_bind);
	wl_global_create(wlc_get_wl_display(), &screensaver_interface, 1, NULL,
				screensaver_bind);
}
