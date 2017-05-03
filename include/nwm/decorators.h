/* Generated by wayland-scanner 1.12.0 */

#ifndef DECORATORS_CLIENT_PROTOCOL_H
#define DECORATORS_CLIENT_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include "wayland-client.h"

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * @page page_decorators The decorators protocol
 * @section page_ifaces_decorators Interfaces
 * - @subpage page_iface_decorator - Decorator protocol for undecorated windows
 */
struct decorator;
struct wl_surface;

/**
 * @page page_iface_decorator decorator
 * @section page_iface_decorator_desc Description
 *
 * Provides a standardized way for nyarlathotep to draw decorators around x windows which 
 * do not have decorators
 * @section page_iface_decorator_api API
 * See @ref iface_decorator.
 */
/**
 * @defgroup iface_decorator The decorator interface
 *
 * Provides a standardized way for nyarlathotep to draw decorators around x windows which 
 * do not have decorators
 */
extern const struct wl_interface decorator_interface;

#ifndef DECORATOR_DECORATEE_STATE_ENUM
#define DECORATOR_DECORATEE_STATE_ENUM
enum decorator_decoratee_state {
	DECORATOR_DECORATEE_STATE_RESIZEABLE = 2,
	DECORATOR_DECORATEE_STATE_CLOSEABLE = 4,
	DECORATOR_DECORATEE_STATE_MAXIMIZABLE = 8,
	DECORATOR_DECORATEE_STATE_MINIMIZABLE = 16,
	DECORATOR_DECORATEE_STATE_TITLEABLE = 32,
	DECORATOR_DECORATEE_STATE_MOVEABLE = 64,
};
#endif /* DECORATOR_DECORATEE_STATE_ENUM */

/**
 * @ingroup iface_decorator
 * @struct decorator_listener
 */
struct decorator_listener {
	/**
	 * another decorator is already running
	 *
	 * Another decorator is already running.
	 */
	void (*decorator_already_present)(void *data,
					  struct decorator *decorator,
					  uint32_t pid);
	/**
	 * called when view with no internal decorator is created
	 *
	 * Called when view with no internal decorator is created
	 */
	void (*undecorated_view_created)(void *data,
					 struct decorator *decorator,
					 uint32_t handle,
					 const char *title,
					 uint32_t decoratee_state);
	/**
	 */
	void (*decoratee_state_changed)(void *data,
					struct decorator *decorator,
					uint32_t handle,
					uint32_t decoratee_state);
	/**
	 */
	void (*decoratee_title_changed)(void *data,
					struct decorator *decorator,
					uint32_t handle,
					const char *title);
	/**
	 */
	void (*decoratee_destroyed)(void *data,
				    struct decorator *decorator,
				    uint32_t handle);
};

/**
 * @ingroup decorator_iface
 */
static inline int
decorator_add_listener(struct decorator *decorator,
		       const struct decorator_listener *listener, void *data)
{
	return wl_proxy_add_listener((struct wl_proxy *) decorator,
				     (void (**)(void)) listener, data);
}

#define DECORATOR_SET_DECORATOR 0
#define DECORATOR_SET_DECORATOR_SURFACE 1
#define DECORATOR_DECORATEE_CLOSED_REQUESTED 2
#define DECORATOR_DECORATEE_MINIMIZE_REQUESTED 3
#define DECORATOR_DECORATEE_MAXIMIZE_REQUESTED 4
#define DECORATOR_DECORATEE_SET_GEOMETRY 5

/**
 * @ingroup iface_decorator
 */
#define DECORATOR_DECORATOR_ALREADY_PRESENT_SINCE_VERSION 1
/**
 * @ingroup iface_decorator
 */
#define DECORATOR_UNDECORATED_VIEW_CREATED_SINCE_VERSION 1
/**
 * @ingroup iface_decorator
 */
#define DECORATOR_DECORATEE_STATE_CHANGED_SINCE_VERSION 1
/**
 * @ingroup iface_decorator
 */
#define DECORATOR_DECORATEE_TITLE_CHANGED_SINCE_VERSION 1
/**
 * @ingroup iface_decorator
 */
#define DECORATOR_DECORATEE_DESTROYED_SINCE_VERSION 1

/**
 * @ingroup iface_decorator
 */
#define DECORATOR_SET_DECORATOR_SINCE_VERSION 1
/**
 * @ingroup iface_decorator
 */
#define DECORATOR_SET_DECORATOR_SURFACE_SINCE_VERSION 1
/**
 * @ingroup iface_decorator
 */
#define DECORATOR_DECORATEE_CLOSED_REQUESTED_SINCE_VERSION 1
/**
 * @ingroup iface_decorator
 */
#define DECORATOR_DECORATEE_MINIMIZE_REQUESTED_SINCE_VERSION 1
/**
 * @ingroup iface_decorator
 */
#define DECORATOR_DECORATEE_MAXIMIZE_REQUESTED_SINCE_VERSION 1
/**
 * @ingroup iface_decorator
 */
#define DECORATOR_DECORATEE_SET_GEOMETRY_SINCE_VERSION 1

/** @ingroup iface_decorator */
static inline void
decorator_set_user_data(struct decorator *decorator, void *user_data)
{
	wl_proxy_set_user_data((struct wl_proxy *) decorator, user_data);
}

/** @ingroup iface_decorator */
static inline void *
decorator_get_user_data(struct decorator *decorator)
{
	return wl_proxy_get_user_data((struct wl_proxy *) decorator);
}

static inline uint32_t
decorator_get_version(struct decorator *decorator)
{
	return wl_proxy_get_version((struct wl_proxy *) decorator);
}

/** @ingroup iface_decorator */
static inline void
decorator_destroy(struct decorator *decorator)
{
	wl_proxy_destroy((struct wl_proxy *) decorator);
}

/**
 * @ingroup iface_decorator
 *
 * This process requests that it will be handling all undecorated windows by providing decorating surfaces.
 */
static inline void
decorator_set_decorator(struct decorator *decorator)
{
	wl_proxy_marshal((struct wl_proxy *) decorator,
			 DECORATOR_SET_DECORATOR);
}

/**
 * @ingroup iface_decorator
 *
 * This designated surface is decorater over view with handle 'handle'
 */
static inline void
decorator_set_decorator_surface(struct decorator *decorator, uint32_t handle, struct wl_surface *surface)
{
	wl_proxy_marshal((struct wl_proxy *) decorator,
			 DECORATOR_SET_DECORATOR_SURFACE, handle, surface);
}

/**
 * @ingroup iface_decorator
 *
 * User clicked on close button
 */
static inline void
decorator_decoratee_closed_requested(struct decorator *decorator, uint32_t handle)
{
	wl_proxy_marshal((struct wl_proxy *) decorator,
			 DECORATOR_DECORATEE_CLOSED_REQUESTED, handle);
}

/**
 * @ingroup iface_decorator
 *
 * User clicked on minimize button
 */
static inline void
decorator_decoratee_minimize_requested(struct decorator *decorator, uint32_t handle)
{
	wl_proxy_marshal((struct wl_proxy *) decorator,
			 DECORATOR_DECORATEE_MINIMIZE_REQUESTED, handle);
}

/**
 * @ingroup iface_decorator
 *
 * User clicked on maximize button
 */
static inline void
decorator_decoratee_maximize_requested(struct decorator *decorator, uint32_t handle)
{
	wl_proxy_marshal((struct wl_proxy *) decorator,
			 DECORATOR_DECORATEE_MAXIMIZE_REQUESTED, handle);
}

/**
 * @ingroup iface_decorator
 *
 * Informs server that decoratee surface should have this position and size.
 */
static inline void
decorator_decoratee_set_geometry(struct decorator *decorator, uint32_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	wl_proxy_marshal((struct wl_proxy *) decorator,
			 DECORATOR_DECORATEE_SET_GEOMETRY, handle, x, y, width, height);
}

#ifdef  __cplusplus
}
#endif

#endif
