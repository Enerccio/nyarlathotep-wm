/**
 * Simple decorator, no themes, nothing
 */

#include "necronomicon.h"
#include "decorators.inc"

struct wl_display* display = NULL;
struct wl_compositor* compositor = NULL;
struct decorator* decorator = NULL;
struct wl_shell* shell;
struct wl_shm* shm;

uint32_t decorator_pid = 0;

static void global_registry_handler(void* data, struct wl_registry* registry,
		uint32_t id, const char *interface, uint32_t version) {
	printf("Got a registry event for %s id %d\n", interface, id);
	if (strcmp(interface, "wl_compositor") == 0) {
		compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 1);
	}

	if (strcmp(interface, "decorator") == 0) {
		decorator = wl_registry_bind(registry, id, &decorator_interface, 1);
	}

	if (strcmp(interface, "wl_shell") == 0) {
		shell = wl_registry_bind(registry, id,
		                                 &wl_shell_interface, 1);
	}

	if (strcmp(interface, "wl_shm") == 0) {
		shm = wl_registry_bind(registry, id,
										 &wl_shm_interface, 1);
	}
}

static void global_registry_remover(void* data, struct wl_registry* registry,
		uint32_t id) {

}

static void undecorated_view_created(void *data, struct decorator *decorator,
		uint32_t handle, const char *title, uint32_t decoratee_state) {
	register_decoration(create_decoration(handle, decoratee_state, title));
}

static void decoratee_state_changed(void *data, struct decorator *decorator,
		uint32_t handle, uint32_t decoratee_state) {

}

static void decoratee_title_changed(void *data, struct decorator *decorator,
		uint32_t handle, const char *title) {

}

static void decoratee_destroyed(void *data, struct decorator *decorator,
		uint32_t handle) {

}

static void decorator_already_present(void *data, struct decorator *decorator, uint32_t pid) {
	decorator_pid = pid;
}

static const struct wl_registry_listener registry_implementation = {
		.global = global_registry_handler,
		.global_remove = global_registry_remover
};

static struct decorator_listener decorator_implementation = {
		.undecorated_view_created = undecorated_view_created,
		.decoratee_state_changed = decoratee_state_changed,
		.decoratee_title_changed = decoratee_title_changed,
		.decoratee_destroyed = decoratee_destroyed,
		.decorator_already_present = decorator_already_present,
};

int main(void) {
	display = wl_display_connect(NULL);
	if (display == NULL) {
		fprintf(stderr,
				"Failed to connect to display. Is wayland server running?\n");
		return EXIT_FAILURE;
	}

	init_registry();
	init_render();

	syslog(LOG_INFO, "necronomicon decorator started");

	struct wl_registry* registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &registry_implementation, NULL);

	wl_display_dispatch(display);
	wl_display_roundtrip(display);

	decorator_add_listener(decorator, &decorator_implementation, NULL);

	// register itself as a decorator
	decorator_set_decorator(decorator);

	wl_display_dispatch(display);
	wl_display_roundtrip(display);

	if (decorator_pid > 0) {
		fprintf(stderr,
				"Another instance of decorator is running with pid: %i. Terminating.\n", decorator_pid);
		return EXIT_FAILURE;
	}

	while (true) {
		for_all(swap_buffers_decoration);
		wl_display_dispatch(display);
	}

	destroy_render();
	wl_display_disconnect(display);

	return EXIT_SUCCESS;
}
