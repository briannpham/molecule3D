#pragma once
#include <emscripten/html5.h>
#include <emscripten/emscripten.h>
#include <GLES3/gl3.h>
#include "math.h"
#include "molecule.h"

// Appearance Settings
extern float g_atom_display_scale_factor; // Default atom scale factor
extern Vec3 bond_color; // Default grey for bonds
extern float bond_radius_scale; // Default bond radius

// Multiple Bond Rendering Parameters
extern const float DEFAULT_BOND_RADIUS;
extern const float DOUBLE_BOND_CYLINDER_RADIUS_SCALE;
extern const float DOUBLE_BOND_OFFSET_FACTOR;
extern const float TRIPLE_BOND_CYLINDER_RADIUS_SCALE;
extern const float TRIPLE_BOND_OFFSET_FACTOR;

// Global WebGL context, shader program, matrices, and uniform locations
extern EMSCRIPTEN_WEBGL_CONTEXT_HANDLE gl_context;
extern GLuint shader_program;
extern GLint position_attribute_location;
extern GLint normal_attribute_location;

extern Mat4 projection_matrix;
extern Mat4 view_matrix;

extern GLint u_model_matrix_loc;
extern GLint u_view_matrix_loc;
extern GLint u_projection_matrix_loc;
extern GLint u_color_loc;
extern GLint u_normal_matrix_loc;

// VAO/VBO for the sphere mesh
extern GLuint sphere_vao;
extern GLuint sphere_vbo_vertices;
extern GLuint sphere_vbo_indices;

// VAO/VBO for the cylinder mesh
extern GLuint cylinder_vao;
extern GLuint cylinder_vbo_vertices;
extern GLuint cylinder_vbo_indices;

extern Molecule current_molecule; // Store the molecule globally for rendering
extern Representation current_representation;

// Functions
void setup_sphere_geometry();
void setup_cylinder_geometry();
Mat4 align_yaxis_to_vector(const Vec3& target_dir_normalized);
void draw_one_cylinder_internal(const Mat4& model_matrix_bond);
void render_frame();

// Emscripten exported functions
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void set_atom_display_scale(float scale);

    EMSCRIPTEN_KEEPALIVE
    void set_bond_radius_value(float radius);

    EMSCRIPTEN_KEEPALIVE
    void set_zoom_level(float zoom);

    EMSCRIPTEN_KEEPALIVE
    void set_auto_rotate(int enabled);

    EMSCRIPTEN_KEEPALIVE
    void set_representation(int rep_value);

    EMSCRIPTEN_KEEPALIVE
    void update_projection_matrix_aspect(int width, int height);

    EMSCRIPTEN_KEEPALIVE
    const char* get_current_molecule_name();

    EMSCRIPTEN_KEEPALIVE
    const char* get_current_molecule_formula();
} 