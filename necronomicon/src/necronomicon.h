/*
 * necronomicon.h
 *
 *  Created on: May 2, 2017
 *      Author: pvan
 */

#ifndef NECRONOMICON_H_
#define NECRONOMICON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/syslog.h>

#include <wayland-client.h>
#include <wayland-client-protocol.h>

#include <nwm/decorators.h>

#include <unistd.h>
#include <sys/mman.h>

#define ERROR_MEM(desc) _ERROR("memory failure", desc)

#define ERROR(desc) _ERROR("failure", desc)

#define _ERROR(type, desc) do { \
	fprintf(stderr, type ": " desc); \
	abort(); \
} while (0)

#define BUTTON_CLOSE (0)
#define BUTTON_MAX   (1)
#define BUTTON_MIN   (2)

#define NUM_BUTTONS  (3)

extern struct wl_shell* shell;
extern struct wl_compositor* compositor;
extern struct wl_display* display;
extern struct wl_shm* shm;

struct render_context;

struct geometry {
	size_t x, y, w, h;
};

struct button {
	struct geometry geometry;
	bool pressed, hovered;
};

struct decoration {
	uint32_t handle;
	bool need_redraw;

	struct wl_surface* surface;
	struct wl_shell_surface* shell_surface;

	enum decorator_decoratee_state state;
	const char* title;
	size_t width, height;

	struct geometry panel;
	struct button button_placement[NUM_BUTTONS];

	struct render_context* context;
};

// registry.c
void init_registry();
bool has_decoration(uint32_t handle);
struct decoration* get_decoration(uint32_t handle);
void register_decoration(struct decoration* decoration);
void remove_decoration(uint32_t handle);
void for_all(void (*func)(struct decoration* decoration));

// decoration.c;
struct decoration* create_decoration(uint32_t handle, enum decorator_decoratee_state state, const char* title);
void destroy_decoration(struct decoration* decoration);

// render.c
void valid_format(void *data, struct wl_shm *wl_shm, uint32_t format);
void init_render();
void destroy_render();
void init_decoration(struct decoration* decoration);
void draw_decoration(struct decoration* decoration);
void swap_buffers_decoration(struct decoration* decoration);

static inline char* copy_string(const char* src) {
	char* cpy = malloc(sizeof(char) * (strlen(src) + 1));
	if (cpy != NULL)
		strcpy(cpy, src);
	return cpy;
}

#endif /* NECRONOMICON_H_ */
