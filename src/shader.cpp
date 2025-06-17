#include "shader.h"
#include <iostream>
#include <vector>

// Updated Vertex Shader
const char* vertex_shader_source = R"glsl(#version 300 es
    uniform mat4 uModelMatrix;
    uniform mat4 uViewMatrix;
    uniform mat4 uProjectionMatrix;
    uniform mat3 uNormalMatrix; // transpose(inverse(uModelMatrix)) for normals

    in vec3 aPosition;
    in vec3 aNormal;

    out vec3 vNormal_world;
    out vec3 vPosition_world; // For specular or other effects later

    void main() {
        vec4 worldPos = uModelMatrix * vec4(aPosition, 1.0);
        vPosition_world = worldPos.xyz;
        gl_Position = uProjectionMatrix * uViewMatrix * worldPos;
        vNormal_world = normalize(uNormalMatrix * aNormal);
    }
)glsl";

// Updated Fragment Shader
const char* fragment_shader_source = R"glsl(#version 300 es
    precision mediump float;
    
    uniform vec4 uColor;

    in vec3 vNormal_world;
    in vec3 vPosition_world; // For specular or other effects later

    out vec4 fragColor;

    void main() {
        vec3 lightDir_world = normalize(vec3(0.5, 0.8, 1.0)); // Light direction in world space
        vec3 normal_world_normalized = normalize(vNormal_world);
        float diffuse_intensity = max(dot(normal_world_normalized, lightDir_world), 0.0);
        float ambient_intensity = 0.25;
        vec3 litColor = uColor.rgb * (ambient_intensity + diffuse_intensity);
        fragColor = vec4(litColor, uColor.a);
    }
)glsl";

GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint log_size = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
        std::vector<GLchar> error_log(log_size);
        glGetShaderInfoLog(shader, log_size, &log_size, &error_log[0]);
        std::cerr << "Shader compilation failed: " << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "\n" << &error_log[0] << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint create_shader_program(const char* vs_source, const char* fs_source) {
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vs_source);
    if (!vs) return 0;
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_source);
    if (!fs) { glDeleteShader(vs); return 0; }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLint log_size = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);
        std::vector<GLchar> error_log(log_size);
        glGetProgramInfoLog(program, log_size, &log_size, &error_log[0]);
        std::cerr << "Shader program linking failed:\n" << &error_log[0] << std::endl;
        glDeleteProgram(program);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
} 