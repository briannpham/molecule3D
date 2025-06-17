// main.cpp - Modular molecular visualization application
#include <iostream>
#include <emscripten/html5.h>
#include <emscripten/emscripten.h>
#include <GLES3/gl3.h>

// Include all modular headers
#include "math.h"
#include "molecule.h"
#include "geometry.h"
#include "shader.h"
#include "input.h"
#include "renderer.h"
#include "parser.h"

int main() {
    std::cout << "Molecular Viewer: Rendering Atoms as Spheres..." << std::endl;

    EMSCRIPTEN_RESULT r = emscripten_set_canvas_element_size("#canvas", 600, 400);
    if (r != EMSCRIPTEN_RESULT_SUCCESS) {
        std::cerr << "Failed to set canvas element size. Result: " << r << std::endl;
    }

    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2; attrs.minorVersion = 0; attrs.alpha = EM_TRUE;
    attrs.depth = EM_TRUE; attrs.stencil = EM_TRUE; attrs.antialias = EM_TRUE;

    gl_context = emscripten_webgl_create_context("#canvas", &attrs);
    if (!gl_context) { std::cerr << "Failed to create WebGL context." << std::endl; return 1; }
    emscripten_webgl_make_context_current(gl_context);
    
    shader_program = create_shader_program(vertex_shader_source, fragment_shader_source);
    if (!shader_program) { std::cerr << "Failed to create shader program." << std::endl; return 1; }
    glUseProgram(shader_program);

    position_attribute_location = glGetAttribLocation(shader_program, "aPosition");
    normal_attribute_location = glGetAttribLocation(shader_program, "aNormal");
    u_model_matrix_loc = glGetUniformLocation(shader_program, "uModelMatrix");
    u_view_matrix_loc = glGetUniformLocation(shader_program, "uViewMatrix");
    u_projection_matrix_loc = glGetUniformLocation(shader_program, "uProjectionMatrix");
    u_color_loc = glGetUniformLocation(shader_program, "uColor");
    u_normal_matrix_loc = glGetUniformLocation(shader_program, "uNormalMatrix");

    if(position_attribute_location == -1 || u_model_matrix_loc == -1 || u_view_matrix_loc == -1 || u_projection_matrix_loc == -1 || u_color_loc == -1 || u_normal_matrix_loc == -1) {
        std::cerr << "Error getting attribute or uniform locations." << std::endl;
        // Optionally print which one failed.
    }
    
    projection_matrix = Mat4::perspective(PI / 3.0f, 600.0f / 400.0f, 0.1f, 100.0f);
    current_molecule = create_sample_molecule(); 
    
    setup_sphere_geometry(); // Create and set up sphere VAO/VBOs
    setup_cylinder_geometry(); // Add this call
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); // Optional: cull back faces for spheres
    glCullFace(GL_BACK);

    // Setup Emscripten mouse and wheel event callbacks
    emscripten_set_mousedown_callback("#canvas", NULL, 1, mousedown_callback);
    emscripten_set_mouseup_callback("#canvas", NULL, 1, mouseup_callback);
    emscripten_set_mousemove_callback("#canvas", NULL, 1, mousemove_callback);
    emscripten_set_wheel_callback("#canvas", NULL, 1, wheel_callback);
    
    std::cout << "Mouse and wheel event callbacks registered for #canvas." << std::endl;

    emscripten_set_main_loop(render_frame, 0, 1);
    std::cout << "Main loop started. Waiting for JS to set initial canvas size and projection." << std::endl;
    return 0;
} 