
#include <wlc/wlc.h>
#include <wlc/wlc-wayland.h>
#include "../nyarlathotep.h"

#include "../../generated/headers/decorators.h"
#include "../../generated/includes/decorators.inc"

static void destroy(struct wl_resource* resource) {
	unregister_decorator(resource);
}

static void set_decorator (struct wl_client* client, struct wl_resource* resource) {
	LOG_DEBUG("set_decorator called");
	uint32_t pid = register_decorator(client, resource);
	if (pid > 0) {
		decorator_send_decorator_already_present(resource, pid);
	} else {
		// TODO: remove, only for test
//		decorator_send_undecorated_view_created(resource, 0, "Hello,  world!", DECORATOR_DECORATEE_STATE_CLOSEABLE
//				| DECORATOR_DECORATEE_STATE_MAXIMIZABLE | DECORATOR_DECORATEE_STATE_MINIMIZABLE);
	}
}

static void set_decorator_surface(struct wl_client* client, struct wl_resource* resource, uint32_t handle,
		struct wl_resource* surface) {
	LOG_DEBUG("set_decorator_surface called");
}

static void decoratee_closed_requested(struct wl_client* client,
		   struct wl_resource* resource,
		   uint32_t handle) {
	LOG_DEBUG("decoratee_closed_requested called");
}

static void decoratee_minimize_requested(struct wl_client* client,
	     struct wl_resource* resource,
	     uint32_t handle) {
	LOG_DEBUG("decoratee_minimize_requested called");
}

static void decoratee_maximize_requested(struct wl_client* client,
	     struct wl_resource* resource,
	     uint32_t handle) {
	LOG_DEBUG("decoratee_maximize_requested called");
}

static void decoratee_set_geometry(struct wl_client* client,
	       struct wl_resource* resource,
	       uint32_t handle,
	       uint32_t x,
	       uint32_t y,
	       uint32_t width,
	       uint32_t height) {
	LOG_DEBUG("decoratee_set_geometry called");
}

static struct decorator_interface decorator_implementation = {
		.set_decorator = set_decorator,
		.set_decorator_surface = set_decorator_surface,
		.decoratee_closed_requested = decoratee_closed_requested,
		.decoratee_minimize_requested = decoratee_minimize_requested,
		.decoratee_maximize_requested = decoratee_maximize_requested,
		.decoratee_set_geometry = decoratee_set_geometry
};

static void decorator_extension_bind(struct wl_client *client, void *data,
		unsigned int version, unsigned int id) {
	if (version != 1) {
		LOG_FATAL("only version 1 supported");
		return;
	}

	struct wl_resource* resource = wl_resource_create(client, &decorator_interface, version, id);
	if (!resource) {
		wl_client_post_no_memory(client);
	}

	wl_resource_set_implementation(resource, &decorator_implementation, NULL, destroy);
}

void register_decorator_extension() {
	wl_global_create(wlc_get_wl_display(), &decorator_interface, 1, NULL,
				decorator_extension_bind);
}
