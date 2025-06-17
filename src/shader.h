#pragma once
#include <GLES3/gl3.h>

// Shader source code
extern const char* vertex_shader_source;
extern const char* fragment_shader_source;

// Functions
GLuint compile_shader(GLenum type, const char* source);
GLuint create_shader_program(const char* vs_source, const char* fs_source); 