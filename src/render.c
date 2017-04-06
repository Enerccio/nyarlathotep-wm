#include <GLES2/gl2.h>

#include "stb/stb_image.h"
#include "pinion.h"
#include "workspace.h"

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

#ifndef __STRING
#define __STRING(x) #x
#endif

#define GL_CALL(x) x; gl_call(__PRETTY_FUNCTION__, __LINE__, __STRING(x))

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
      sprintf(msg, "Compiling %s: %*s\n", (shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment"), len, log);

      logger_callback(WLC_LOG_ERROR, msg);
      abort();
   }

   return shader;
}

void context_open(wlc_handle output) {
	workspace_t* workspace = get_workspace_for_output(output);
	if (workspace == NULL)
		return;

	char* vertex_shader = "#version 110\n"
				"precision mediump float;\n"
				"\n"
				"uniform vec2 resolution;\n"
				"uniform vec2 origin;\n"
				"attribute vec4 position;\n"
				"attribute vec2 uv;\n"
				"varying vec2 v_uv;\n"
				"\n"
				"void main() {\n"
				"	mat4 ortho = mat4(\n"
				"		2.0/resolution.x,         0,          0,      0,\n"
				"		0,        -2.0/resolution.y,          0,      0,\n"
				"		0,                        0,         -1,      0,\n"
				"		-1,                       1,          0,      1\n"
				"	);\n"
				"	gl_Position = ortho * (vec4(origin, 0, 0) + position);\n"
				"	v_uv = uv;\n"
				"}\n";
	char* fragment_shader = "#version 110\n"
				"precision mediump float;\n"
				"\n"
				"varying vec2 v_uv;\n"
				"uniform sampler2D texture;\n"
				"\n"
				"void main() {\n"
				"	vec4 color = texture2D(texture, v_uv);\n"
				"	gl_FragColor = color;\n"
				"}\n";

	GLuint vertf = create_shader(vertex_shader, GL_VERTEX_SHADER);
	GLuint fragf = create_shader(fragment_shader, GL_FRAGMENT_SHADER);

	workspace->background_shader = glCreateProgram();
	GL_CALL(glAttachShader(workspace->background_shader, vertf));
	GL_CALL(glAttachShader(workspace->background_shader, fragf));
	GL_CALL(glLinkProgram(workspace->background_shader));
	GL_CALL(glDeleteShader(vertf));
	GL_CALL(glDeleteShader(fragf));

	const char* source_background = get_background();
	int32_t x, y, channels, reqchannels = 3;
	uint8_t* background = (uint8_t*) stbi_load(source_background, &x, &y,
			&channels, reqchannels);
	if (background == NULL)
		return;

	uint8_t* data;
	if (channels != 3) {
		data = malloc(x * y * 3);
		if (data == NULL)
			return;

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

	GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	GL_CALL(glGenTextures(1, &workspace->background_texture));

	GL_CALL(glBindTexture(GL_TEXTURE_2D, workspace->background_texture));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB,
			GL_UNSIGNED_BYTE, data));

	free(data);
}

void context_closed(wlc_handle output) {
	workspace_t* workspace = get_workspace_for_output(output);
	if (workspace == NULL)
		return;

	GL_CALL(glDeleteTextures(1, &workspace->background_texture));
	GL_CALL(glDeleteProgram(workspace->background_shader));

	workspace->background_shader = workspace->background_texture = 0;
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

void uv_vertices(float* uvs, const float uv1a, const float uv1b, const float uv2a, const float uv2b) {
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

static void render_rectangle(wlc_handle output, GLuint texture, GLuint program,
		uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
	const struct wlc_size* render_size = wlc_output_get_resolution(output);

	float vertices[24];
	rectangle_vertices(vertices, w, h);
	float uvs[12];
	uv_vertices(uvs, 0, 0, 1, 1);

	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GL_CALL(glUseProgram(program));

	GLuint attributePosition = GL_CALL(glGetAttribLocation(program, "position"));
	GLuint attributeUV = GL_CALL(glGetAttribLocation(program, "uv"));
	GLuint uniformResolution = GL_CALL(glGetUniformLocation(program, "resolution"));
	GLuint uniformOrigin = GL_CALL(glGetUniformLocation(program, "origin"));
	GLuint uniformTexture = GL_CALL(glGetUniformLocation(program, "texture"));

	GL_CALL(glEnableVertexAttribArray(attributePosition));
	GL_CALL(glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, vertices));
	GL_CALL(glEnableVertexAttribArray(attributeUV));
	GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, uvs));

	GL_CALL(glActiveTexture(GL_TEXTURE0));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
	GL_CALL(glUniform1i(uniformTexture, 0));

	GL_CALL(glUniform2f(uniformResolution, render_size->w, render_size->h));
	GL_CALL(glUniform2f(uniformOrigin, x, y));

	GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));

	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glUseProgram(0));
}

void custom_render(wlc_handle output) {
	workspace_t* workspace = get_workspace_for_output(output);
	if (workspace == NULL)
		return;

	GL_CALL(glEnable(GL_BLEND));
	GL_CALL(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
	GL_CALL(glClearColor(0.0f, 0.0f, 0.1f, 1.0f));
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

	// background
	if (workspace->background_texture != 0) {
		render_rectangle(output, workspace->background_texture, workspace->background_shader,
				0, 0, workspace->w, workspace->h);
	}
}
