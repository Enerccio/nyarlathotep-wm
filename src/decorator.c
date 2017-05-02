#include "nyarlathotep.h"

#include "libds/hmap.h"

struct wl_client* decorator = NULL;
struct wl_resource* decorator_connection = NULL;

uint32_t register_decorator(struct wl_client* client, struct wl_resource* connection) {
	if (decorator == NULL) {
		decorator = client;
		decorator_connection = connection;
		return 0;
	}
	uint32_t pid;
	wl_client_get_credentials(decorator, &pid, NULL, NULL);
	return pid;
}

void unregister_decorator(struct wl_resource* connection) {
	if (connection == decorator_connection) {
		decorator = NULL;
		decorator_connection = NULL;
	}
}
