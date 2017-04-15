#include <GLES2/gl2.h>

#include "shaders.def"

#include "stb/stb_image.h"
#include <wlc/wlc-wayland.h>
#include <wlc/wlc-render.h>

#include "nyarlathotep.h"
#include "workspace.h"
#include "utils.h"

float* vertices;
float* uvs;

void init_render() {
	vertices = malloc(sizeof(float)*24);
	uvs = malloc(sizeof(float)*12);
}

static const char*
gl_error_string(const GLenum error) {
	switch (error) {
	case GL_INVALID_ENUM:
		return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:
		return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION:
		return "GL_INVALID_OPERATION";
	case GL_OUT_OF_MEMORY:
		return "GL_OUT_OF_MEMORY";
	}

	return "UNKNOWN GL ERROR";
}

#define GL_CALL(x) x; gl_call(__PRETTY_FUNCTION__, __LINE__, STRINGIZE(x))

static
void gl_call(const char *func, uint32_t line, const char *glfunc) {
	GLenum error;
	if ((error = glGetError()) == GL_NO_ERROR)
		return;

	char msg[2048];
	sprintf(msg, "gles2: function %s at line %u: %s == %s", func, line, glfunc,
			gl_error_string(error));

	logger_callback(WLC_LOG_ERROR, msg);
}

static GLuint create_shader(const char *source, GLenum shader_type) {
	GLuint shader = glCreateShader(shader_type);
	if (shader == 0)
		abort();

	GL_CALL(glShaderSource(shader, 1, (const char**)&source, NULL));
	GL_CALL(glCompileShader(shader));

	GLint status;
	GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &status));
	if (!status) {
		GLsizei len;
		char log[1024];
		GL_CALL(glGetShaderInfoLog(shader, sizeof(log), &len, log));

		char msg[2048];
		sprintf(msg, "Compiling %s: %*s\n",
				(shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment"), len,
				log);

		logger_callback(WLC_LOG_ERROR, msg);
		abort();
	}

	return shader;
}

static GLuint compile_shader(char* vertex_shader, char* fragment_shader) {
	GLuint program = 0;
	GLuint vertf = create_shader(vertex_shader, GL_VERTEX_SHADER);
	GLuint fragf = create_shader(fragment_shader, GL_FRAGMENT_SHADER);

	program = glCreateProgram();
	GL_CALL(glAttachShader(program, vertf));
	GL_CALL(glAttachShader(program, fragf));
	GL_CALL(glLinkProgram(program));
	GL_CALL(glDeleteShader(vertf));
	GL_CALL(glDeleteShader(fragf));

	GLint status;
	GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, &status));
	if (!status) {
		GLsizei len;
		char log[1024];
		GL_CALL(glGetProgramInfoLog(program, sizeof(log),
						&len, log));
		char msg[2048];
		sprintf(msg, "Linking:\n%*s\n", len, log);

		logger_callback(WLC_LOG_ERROR, msg);

		abort();
	}

	return program;
}

#define SHADER_LIST_SIZE (8)

static void compile_shaders(workspace_t* workspace) {
	workspace->render_mode_shaders = malloc(sizeof(GLuint) * SHADER_LIST_SIZE);
	ASSERT_MEM(workspace->render_mode_shaders);
	workspace->plane_color_shader = compile_shader(__vertex_shader_plane, __fragment_shader_plane_color);

	workspace->render_mode_shaders[SURFACE_RGB] = compile_shader(__vertex_shader_plane, __fragment_shader_rgb);
	workspace->render_mode_shaders[SURFACE_RGBA] = compile_shader(__vertex_shader_plane, __fragment_shader_rgba);
	workspace->render_mode_shaders[SURFACE_EGL] = compile_shader(__vertex_shader_plane, __fragment_shader_egl);
	workspace->render_mode_shaders[SURFACE_Y_UV] = compile_shader(__vertex_shader_plane, __fragment_shader_y_uv);
	workspace->render_mode_shaders[SURFACE_Y_U_V] = compile_shader(__vertex_shader_plane, __fragment_shader_y_u_v);
	workspace->render_mode_shaders[SURFACE_Y_XUXV] = compile_shader(__vertex_shader_plane, __fragment_shader_y_xuxv);
	workspace->render_mode_shaders[SURFACE_WLC_POINTER] = compile_shader(__vertex_shader_plane, __fragment_shader_wlc_cursor);
	workspace->render_mode_shaders[INVALID] = compile_shader(__vertex_shader_plane, __fragment_shader_dummy);
}

static void load_image(const char* src, GLuint* ptr, bool preserve_alpha) {
	if (src == NULL || strcmp(src, "") == 0)
			return;

	int32_t x, y, channels, reqchannels = (preserve_alpha ? 4 : 3);
	uint8_t* background = (uint8_t*) stbi_load(src, &x, &y,
			&channels, reqchannels);
	if (background == NULL)
		return;

	if (preserve_alpha) {
		uint8_t* data;
		if (channels != 4) {
			data = malloc(x * y * 4);
			ASSERT_MEM(data);

			uint32_t srcptr = 0;
			uint32_t tgtptr = 0;
			for (uint32_t i = 0; i < x; i++)
				for (uint32_t j = 0; j < y; j++) {
					if (channels == 1) {
						data[tgtptr++] = background[srcptr];
						data[tgtptr++] = background[srcptr];
						data[tgtptr++] = background[srcptr];
						data[tgtptr++] = 255;
						++srcptr;
					} else if (channels == 3) {
						data[tgtptr++] = background[srcptr++];
						data[tgtptr++] = background[srcptr++];
						data[tgtptr++] = background[srcptr++];
						data[tgtptr++] = 255;
					}
				}
			free(background);
		} else {
			data = background;
		}

		GL_CALL(glGenTextures(1, ptr));

		if ((*ptr) == 0)
			return;

		GL_CALL(glBindTexture(GL_TEXTURE_2D, *ptr));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));

		free(data);
	} else {
		uint8_t* data;
		if (channels != 3) {
			data = malloc(x * y * 3);
			ASSERT_MEM(data);

			uint32_t srcptr = 0;
			uint32_t tgtptr = 0;
			for (uint32_t i = 0; i < x; i++)
				for (uint32_t j = 0; j < y; j++) {
					if (channels == 1) {
						data[tgtptr++] = background[srcptr];
						data[tgtptr++] = background[srcptr];
						data[tgtptr++] = background[srcptr];
						++srcptr;
					} else if (channels == 4) {
						data[tgtptr++] = background[srcptr++];
						data[tgtptr++] = background[srcptr++];
						data[tgtptr++] = background[srcptr++];
						++srcptr;
					}
				}
			free(background);
		} else {
			data = background;
		}

		GL_CALL(glGenTextures(1, ptr));

		if ((*ptr) == 0)
			return;

		GL_CALL(glBindTexture(GL_TEXTURE_2D, *ptr));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data));

		free(data);
	}
}

void context_open(wlc_handle output) {
	workspace_t* workspace = get_workspace_for_output(output);
	if (workspace == NULL)
		return;

	workspace->context_inited = true;
	compile_shaders(workspace);

	load_image(get_background(), &workspace->background_texture, false);
	load_image(get_cursor_image(), &workspace->cursor_texture, true);
}

void context_closed(wlc_handle output) {
	workspace_t* workspace = get_workspace_for_output(output);
	if (workspace == NULL)
		return;

	if (!workspace->context_inited)
		return;

	GL_CALL(glDeleteTextures(1, &workspace->background_texture));
	for (int i=0; i<SHADER_LIST_SIZE; i++) {
		GL_CALL(glDeleteProgram(workspace->render_mode_shaders[i]));
	}
	free(workspace->render_mode_shaders);
	workspace->background_texture = 0;

	workspace->context_inited = false;
}

void rectangle_vertices(float* vertices, const float w, const float h) {
	vertices[0] = 0;
	vertices[1] = 0;
	vertices[2] = 0;
	vertices[3] = 1;
	vertices[4] = w;
	vertices[5] = 0;
	vertices[6] = 0;
	vertices[7] = 1;
	vertices[8] = w;
	vertices[9] = h;
	vertices[10] = 0;
	vertices[11] = 1;
	vertices[12] = 0;
	vertices[13] = 0;
	vertices[14] = 0;
	vertices[15] = 1;
	vertices[16] = 0;
	vertices[17] = h;
	vertices[18] = 0;
	vertices[19] = 1;
	vertices[20] = w;
	vertices[21] = h;
	vertices[22] = 0;
	vertices[23] = 1;
}

void uv_vertices(float* uvs, const float uv1a, const float uv1b,
		const float uv2a, const float uv2b) {
	uvs[0] = uv1a;
	uvs[1] = uv1b;
	uvs[2] = uv2a;
	uvs[3] = uv1b;
	uvs[4] = uv2a;
	uvs[5] = uv2b;
	uvs[6] = uv1a;
	uvs[7] = uv1b;
	uvs[8] = uv1a;
	uvs[9] = uv2b;
	uvs[10] = uv2a;
	uvs[11] = uv2b;
}

static void render_rectangle(wlc_handle output, GLuint* texture, GLuint program,
		int32_t x, int32_t y, uint32_t w, uint32_t h) {
	const struct wlc_size* render_size = wlc_output_get_resolution(output);

	rectangle_vertices(vertices, w, h);
	uv_vertices(uvs, 0, 0, 1, 1);

	GL_CALL(glUseProgram(program));

	GLuint attributePosition = GL_CALL(glGetAttribLocation(program, "position"));
	GLuint attributeUV = GL_CALL(glGetAttribLocation(program, "uv"));
	GLuint uniformResolution = GL_CALL(glGetUniformLocation(program, "resolution"));
	GLuint uniformOrigin = GL_CALL(glGetUniformLocation(program, "origin"));
	GLuint uniformTexture = GL_CALL(glGetUniformLocation(program, "texture"));
	GLuint uniformTexture2 = GL_CALL(glGetUniformLocation(program, "texture1"));
	GLuint uniformTexture3 = GL_CALL(glGetUniformLocation(program, "texture2"));

	GL_CALL(glEnableVertexAttribArray(attributePosition));
	GL_CALL(glVertexAttribPointer(attributePosition, 4, GL_FLOAT, false, 0, vertices));
	GL_CALL(glEnableVertexAttribArray(attributeUV));
	GL_CALL(glVertexAttribPointer(attributeUV, 2, GL_FLOAT, false, 0, uvs));

	if (uniformTexture < 0xffffffff && texture[0] > 0) {
		GL_CALL(glActiveTexture(GL_TEXTURE0));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, texture[0]));
		GL_CALL(glUniform1i(uniformTexture, 0));
	}

	if (uniformTexture2 < 0xffffffff) {
		GL_CALL(glActiveTexture(GL_TEXTURE1));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, texture[1]));
		GL_CALL(glUniform1i(uniformTexture2, 1));
	}

	if (uniformTexture3 < 0xffffffff) {
		GL_CALL(glActiveTexture(GL_TEXTURE2));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, texture[2]));
		GL_CALL(glUniform1i(uniformTexture2, 2));
	}

	GL_CALL(glUniform2f(uniformResolution, render_size->w, render_size->h));
	GL_CALL(glUniform2f(uniformOrigin, x, y));

	GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));

	GL_CALL(glDisableVertexAttribArray(attributePosition));
	GL_CALL(glDisableVertexAttribArray(attributeUV));
	GL_CALL(glUseProgram(0));
}

static void render_rectangle_color(wlc_handle output, GLuint program,
		int32_t x, int32_t y, uint32_t w, uint32_t h,
		float r, float g, float b, float a) {
	const struct wlc_size* render_size = wlc_output_get_resolution(output);

	rectangle_vertices(vertices, w, h);

	GL_CALL(glUseProgram(program));

	GLuint attributePosition = GL_CALL(glGetAttribLocation(program, "position"));
	GLuint uniformResolution = GL_CALL(glGetUniformLocation(program, "resolution"));
	GLuint uniformOrigin = GL_CALL(glGetUniformLocation(program, "origin"));
	GLuint uniformColor = GL_CALL(glGetUniformLocation(program, "color"));

	GL_CALL(glUniform4f(uniformColor, r, g, b, a));

	GL_CALL(glEnableVertexAttribArray(attributePosition));
	GL_CALL(glVertexAttribPointer(attributePosition, 4, GL_FLOAT, false, 0, vertices));


	GL_CALL(glUniform2f(uniformResolution, render_size->w, render_size->h));
	GL_CALL(glUniform2f(uniformOrigin, x, y));

	GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));

	GL_CALL(glDisableVertexAttribArray(attributePosition));
	GL_CALL(glUseProgram(0));
}

static void render_view_actual(wlc_handle output, wlc_handle view, workspace_t* workspace) {
	uint32_t texture[3];
	enum wlc_surface_format fmt;
	wlc_surface_get_textures(wlc_view_get_surface(view), texture, &fmt);
	const struct wlc_geometry* geo = wlc_view_get_geometry(view);

	render_rectangle(output, texture, workspace->render_mode_shaders[fmt], geo->origin.x,
			geo->origin.y, geo->size.w, geo->size.h);
}

static void render_mouse_pointer(workspace_t* workspace, wlc_handle output) {
	GLuint cursor_texture = workspace->cursor_texture;
	GLuint program = 0;

	GLuint texture[3];
	enum wlc_surface_format format;
	struct wlc_point tip;
	struct wlc_size size;

	if (wlc_get_active_pointer(output, texture, &format, &tip, &size)) {
		// got pointer from wlc
		if (format != INVALID) {
			program = workspace->render_mode_shaders[format];
		} else {
			tip.x = get_cursor_offset()[0];
			tip.y = get_cursor_offset()[1];
			size.w = 32;
			size.h = 32;
			program = workspace->render_mode_shaders[SURFACE_RGBA];
		}
	} else {
		if (format != INVALID) {
			program = workspace->render_mode_shaders[format];
		} else {
			tip.x = get_cursor_offset()[0];
			tip.y = get_cursor_offset()[1];
			size.w = 32;
			size.h = 32;
			program = workspace->render_mode_shaders[SURFACE_RGBA];
		}
	}

	if (texture[0] == 0) {
		texture[0] = cursor_texture;
	}

	fprintf(stdout, "cursor: %i, %i, %i -> %i - %i;\n", texture[0], texture[1], texture[2], program, format);
	fflush(stdout);

	render_rectangle(output, texture,
					program, workspace->px - tip.x,
					workspace->py - tip.y, size.w, size.h);
}

void custom_render(wlc_handle output) {
	GL_CALL(glEnable(GL_BLEND));
	GL_CALL(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
	GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

	workspace_t* workspace = get_workspace_for_output(output);
	if (workspace == NULL)
		return;

	render_rectangle_color(output, workspace->plane_color_shader, 0, 0,
			workspace->w, workspace->h, 0, 0, 0, 1);

	if (workspace->background_texture != 0) {
		render_rectangle(output, &workspace->background_texture,
				workspace->render_mode_shaders[SURFACE_RGB], 0, 0, workspace->w, workspace->h);
	}

	if (workspace->main_view != 0) {
		render_view_actual(output, workspace->main_view, workspace);

		list_iterator_t li;
		list_create_iterator(workspace->floating_windows, &li);

		while (list_has_next(&li)) {
			wlc_handle view = (wlc_handle)list_next(&li);

			if (is_parent_view(workspace->main_view, view)) {
				render_view_actual(output, view, workspace);
			}
		}
	}

	if (workspace->window_list_show_width > 0) {
		const float* window_list_color = get_window_list_color();
		const float* window_list_sel_color = get_window_list_selection_color();

		render_rectangle_color(output, workspace->plane_color_shader,
				workspace->w - workspace->window_list_show_width, 0,
				workspace->window_list_show_width, workspace->h,
				window_list_color[0], window_list_color[1],
				window_list_color[2], window_list_color[3]);

		int win_width = workspace->window_list_total_width * 0.8f;
		int total_win_size = get_side_window_height() + get_size_window_offset();
		int num_hidden_windows = list_size(workspace->hidden_windows);
		int max_scroll = (num_hidden_windows * total_win_size) - workspace->h;

		max_scroll = max_scroll <= 0 ? 0 : max_scroll;

		workspace->window_list_scroll_offset = workspace->window_list_scroll_offset < 0 ?
				0 : workspace->window_list_scroll_offset;
		workspace->window_list_scroll_offset = workspace->window_list_scroll_offset >= max_scroll ?
				max_scroll : workspace->window_list_scroll_offset;

		list_iterator_t li;
		list_create_iterator(workspace->hidden_windows, &li);

		int h = 0;

		while (list_has_next(&li)) {
			int wlx = (workspace->w - workspace->window_list_show_width)
					+ ((workspace->window_list_total_width - win_width)/2);
			int wly = h + ((get_size_window_offset() - get_side_window_height()) / 2)
					- workspace->window_list_scroll_offset;

			wlc_handle view = (wlc_handle)list_next(&li);
			uint32_t texture[3];
			enum wlc_surface_format fmt;
			wlc_surface_get_textures(wlc_view_get_surface(view), texture, &fmt);

			if (view == workspace->window_list_selected_view) {
				render_rectangle_color(output, workspace->plane_color_shader,
								wlx - RENDER_SELECTED_WINDOW_LIST_OFFSET, wly - RENDER_SELECTED_WINDOW_LIST_OFFSET,
								win_width + (2*RENDER_SELECTED_WINDOW_LIST_OFFSET), get_side_window_height() + (2*RENDER_SELECTED_WINDOW_LIST_OFFSET),
								window_list_sel_color[0], window_list_sel_color[1],
								window_list_sel_color[2], window_list_sel_color[3]);
			}

			render_rectangle(output, texture, workspace->render_mode_shaders[fmt], wlx,
							wly, win_width, get_side_window_height());

			h += get_size_window_offset();
		}
	}

	render_mouse_pointer(workspace, output);
}
