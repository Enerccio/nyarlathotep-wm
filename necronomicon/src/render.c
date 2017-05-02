#include "necronomicon.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <cairo/cairo.h>

struct wl_shm_pool;

#define BYTES_PER_PIXEL (2) // RGBA

struct shm_buffer {
	char buffer_file[256];
	int buffer_fd;
	size_t buffer_size;
	void* bytes;

	struct wl_shm_pool* pool;
	struct wl_buffer* buffer;
};

struct cairo_context {
	cairo_t* ctx;
};

struct render_context {
	struct shm_buffer buffer;
	struct cairo_context cairo;
};

int urandom_fd;

uint8_t rand_byte() {
	uint8_t d;
	read(urandom_fd, &d, 1);
	return d;
}

void guid(char GUID[37]) {
	char* template = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
	char* hex = "0123456789ABCDEF-";
	int nLen = strlen(template);

	for (int t=0; t<nLen+1; t++) {
		int r = rand_byte() % 16;
		char c = ' ';

		switch (template[t]) {
			case 'x' : { c = hex [r]; } break;
			case 'y' : { c = hex [(r & 0x03) | 0x08]; } break;
			case '-' : { c = '-'; } break;
			case '4' : { c = '4'; } break;
		}

		GUID[t] = ( t < nLen ) ? c : 0x00;
	}
}

static void destroy_shm_buffer(struct shm_buffer* b) {
	// TODO
}

static void create_shm_buffer(uint32_t handle, struct shm_buffer* b, int w, int h) {
	if (b->buffer_file != NULL)
		destroy_shm_buffer(b);

	char GUID[37];
	guid(GUID);

	sprintf(b->buffer_file, "/tmp/necronomicon/%i_%s.shm", handle, GUID);

	b->buffer_fd = open(b->buffer_file, O_RDWR | O_CREAT);
	if (b->buffer_fd < 0)
		ERROR("failed to create shm buffer file (max open fd?)");

	b->buffer_size = w * h * BYTES_PER_PIXEL;

	if (ftruncate(b->buffer_fd, b->buffer_size) < 0)
		ERROR("failed to truncate shm file");

	b->bytes = mmap(NULL, b->buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, b->buffer_fd, 0);

	if (b->bytes == MAP_FAILED)
		ERROR("memmap failure");

	unlink(b->buffer_file);

	b->pool = wl_shm_create_pool(shm, b->buffer_fd, b->buffer_size);
	b->buffer = wl_shm_pool_create_buffer(b->pool, 0, w, h, w*BYTES_PER_PIXEL, WL_SHM_FORMAT_RGB565);
}

void init_render() {
	urandom_fd = open("/dev/urandom", O_RDONLY);
	if (urandom_fd < 0) {
		ERROR("failed to open urandom");
	}

	struct stat st = {0};
	if (stat("/tmp/necronomicon", &st) == -1) {
		if (mkdir("/tmp/necronomicon", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
			ERROR("failed to create /tmp/necronomicon directory");
		}
	}
}

void destroy_render() {
	close(urandom_fd);
}

struct render_context* create_cairo_context(struct decoration* decoration) {
	struct render_context* ctx = malloc(sizeof(struct render_context));
	if (ctx == NULL)
		ERROR_MEM("failed to allocate render context");

	memset(ctx, 0, sizeof(struct render_context));

	create_shm_buffer(decoration->handle, &ctx->buffer, 1, 1); // initial size is 1x1 px

	return ctx;
}

void init_decoration(struct decoration* decoration) {
	decoration->context = create_cairo_context(decoration);
}

static void draw_borders(struct decoration* decoration, cairo_t* cr) {
	// outer black border
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_rectangle(cr, 0, 0, decoration->width, decoration->height);
	cairo_fill (cr);

	// inner white border
	cairo_set_source_rgb (cr, 1, 1, 1);
	cairo_rectangle(cr, 1, 1, decoration->width-2, decoration->height-2);
	cairo_fill (cr);

	// inner back border
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_rectangle(cr, 2, 2, decoration->width-4, decoration->height-4);
	cairo_fill (cr);

	// fill white
	cairo_set_source_rgb (cr, 1, 1, 1);
	cairo_rectangle(cr, 3, 3, decoration->width-6, decoration->height-6);
	cairo_fill (cr);
}

static void draw_button(struct decoration* decoration, cairo_t* cr, int button_type, size_t* pos, size_t* remwidth) {
	// draw button base
	if (decoration->button_placement[button_type].pressed) {
		cairo_set_source_rgb (cr, 0.5, 0.5, 1);
	} else if (decoration->button_placement[button_type].hovered ){
		cairo_set_source_rgb (cr, 1, 0.5, 1);
	} else {
		cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);
	}

	cairo_rectangle(cr, *pos, 4, (*pos)+20, 24);

	if (button_type == BUTTON_CLOSE) {
		cairo_set_source_rgb (cr, 0, 0, 0);
		cairo_move_to(cr, (*pos)+1, 5);
		cairo_line_to(cr, (*pos)+19, 23);
		cairo_move_to(cr, (*pos)+1, 23);
		cairo_line_to(cr, (*pos)+19, 5);
	}

	if (button_type == BUTTON_MIN) {
		cairo_set_source_rgb (cr, 0, 0, 0);
		cairo_rectangle(cr, (*pos)+1, 21, 18, 3);
	}

	if (button_type == BUTTON_MAX) {
		cairo_set_source_rgb (cr, 0, 0, 0);
		cairo_rectangle(cr, (*pos)+1, 5, 18, 18);
		cairo_set_source_rgb (cr, 1, 1, 1);
		cairo_rectangle(cr, (*pos)+2, 6, 16, 2);
		cairo_rectangle(cr, (*pos)+2, 9, 16, 14);
	}

	decoration->button_placement[button_type].geometry.h = 20;
	decoration->button_placement[button_type].geometry.w = 20;
	decoration->button_placement[button_type].geometry.x = *pos;
	decoration->button_placement[button_type].geometry.y = 4;

	*pos += 22;
	*remwidth -= 22;
}

static void draw_header(struct decoration* decoration, cairo_t* cr) {
	size_t remwidth = decoration->width - 8;
	size_t pos = 4;

	if ((decoration->state & DECORATOR_DECORATEE_STATE_CLOSEABLE) == DECORATOR_DECORATEE_STATE_CLOSEABLE) {
		draw_button(decoration, cr, BUTTON_CLOSE, &pos, &remwidth);
	}

	if ((decoration->state & DECORATOR_DECORATEE_STATE_MINIMIZABLE) == DECORATOR_DECORATEE_STATE_MINIMIZABLE) {
		draw_button(decoration, cr, BUTTON_MIN, &pos, &remwidth);
	}

	if ((decoration->state & DECORATOR_DECORATEE_STATE_MAXIMIZABLE) == DECORATOR_DECORATEE_STATE_MAXIMIZABLE) {
		draw_button(decoration, cr, BUTTON_MAX, &pos, &remwidth);
	}

	decoration->panel.x = pos;
	decoration->panel.y = 4;
	decoration->panel.w = remwidth;
	decoration->panel.h = 20;
}

void draw_decoration(struct decoration* decoration) {
	decoration->need_redraw = true;

	cairo_surface_t* srfc = cairo_image_surface_create_for_data(decoration->context->buffer.bytes, CAIRO_FORMAT_RGB16_565,
			decoration->width, decoration->height, decoration->width * BYTES_PER_PIXEL);
	cairo_t* cr = cairo_create(srfc);

	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_rectangle(cr, 0, 0, decoration->width, decoration->height);
	cairo_fill (cr);

	if (decoration->width > 80 && decoration->height > 40) {
		// useful sizes, do actually draw stuff
		draw_borders(decoration, cr);
		draw_header(decoration, cr);
	}
}

void swap_buffers_decoration(struct decoration* decoration) {
	if (decoration->need_redraw) {
		struct shm_buffer* buffer = &decoration->context->buffer;

		wl_surface_attach(decoration->surface, buffer->buffer, 0, 0);
		wl_surface_damage(decoration->surface, 0, 0, decoration->width, decoration->height);
	}
	decoration->need_redraw = false;
}
