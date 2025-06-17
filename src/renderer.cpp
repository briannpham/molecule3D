#include "renderer.h"
#include "geometry.h"
#include "input.h"
#include <iostream>
#include <algorithm>

// Appearance Settings
float g_atom_display_scale_factor = 0.65f; // Default atom scale factor
Vec3 bond_color(0.6f, 0.6f, 0.6f); // Default grey for bonds
float bond_radius_scale = 0.10f; // Default bond radius updated to 0.10f

// Multiple Bond Rendering Parameters
const float DEFAULT_BOND_RADIUS = 0.10f; // Reference for bond_radius if reset functionality is ever added
const float DOUBLE_BOND_CYLINDER_RADIUS_SCALE = 0.5f;  // Each cylinder in a double bond is X% of main bond_radius
const float DOUBLE_BOND_OFFSET_FACTOR = 1.0f;         // Increased offset from 0.5f

const float TRIPLE_BOND_CYLINDER_RADIUS_SCALE = 0.33f; // Each cylinder in a triple bond is X% of main bond_radius
const float TRIPLE_BOND_OFFSET_FACTOR = 1.34f;        // Increased offset from 0.67f (approx doubled)

// Global WebGL context, shader program, matrices, and uniform locations
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE gl_context = 0;
GLuint shader_program = 0;
GLint position_attribute_location = -1;
GLint normal_attribute_location = -1; // Added for normals

Mat4 projection_matrix;
Mat4 view_matrix;

GLint u_model_matrix_loc = -1;
GLint u_view_matrix_loc = -1;
GLint u_projection_matrix_loc = -1;
GLint u_color_loc = -1;
GLint u_normal_matrix_loc = -1; // For transforming normals

// VAO/VBO for the sphere mesh
GLuint sphere_vao = 0;
GLuint sphere_vbo_vertices = 0; // For vertex data (pos, norm)
GLuint sphere_vbo_indices = 0;  // For index data

// VAO/VBO for the cylinder mesh
GLuint cylinder_vao = 0;
GLuint cylinder_vbo_vertices = 0;
GLuint cylinder_vbo_indices = 0;

Molecule current_molecule; // Store the molecule globally for rendering
Representation current_representation = Representation::BallAndStick;

void setup_sphere_geometry() {
    create_uv_sphere(1.0f, 32, 32); // Create a unit sphere, will be scaled per atom

    glGenVertexArrays(1, &sphere_vao);
    glBindVertexArray(sphere_vao);

    glGenBuffers(1, &sphere_vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sphere_vertices.size() * sizeof(float), sphere_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &sphere_vbo_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_vbo_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_indices.size() * sizeof(unsigned int), sphere_indices.data(), GL_STATIC_DRAW);

    // Vertex positions
    if (position_attribute_location != -1) {
        glVertexAttribPointer(position_attribute_location, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(position_attribute_location);
    }
    // Vertex normals
    if (normal_attribute_location != -1) {
        glVertexAttribPointer(normal_attribute_location, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(normal_attribute_location);
    }
    glBindVertexArray(0); // Unbind VAO
}

void setup_cylinder_geometry() {
    create_cylinder_mesh(1.0f, 1.0f, 16); // Unit cylinder: radius 1, height 1, 16 segments

    glGenVertexArrays(1, &cylinder_vao);
    glBindVertexArray(cylinder_vao);

    glGenBuffers(1, &cylinder_vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, cylinder_vertices.size() * sizeof(float), cylinder_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &cylinder_vbo_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinder_vbo_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinder_indices.size() * sizeof(unsigned int), cylinder_indices.data(), GL_STATIC_DRAW);

    if (position_attribute_location != -1) {
        glVertexAttribPointer(position_attribute_location, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(position_attribute_location);
    }
    if (normal_attribute_location != -1) {
        glVertexAttribPointer(normal_attribute_location, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(normal_attribute_location);
    }
    glBindVertexArray(0);
}

// Helper function to create rotation matrix to align Y-axis with a given direction vector
Mat4 align_yaxis_to_vector(const Vec3& target_dir_normalized) {
    Vec3 y_axis(0, 1, 0);
    Vec3 rotation_axis = Vec3::cross(y_axis, target_dir_normalized);
    float rotation_angle = std::acos(Vec3::dot(y_axis, target_dir_normalized));
    Mat4 rotation_matrix = Mat4::identity();
    if (std::abs(Vec3::dot(y_axis, target_dir_normalized)) > 0.9999f) { 
        if (Vec3::dot(y_axis, target_dir_normalized) < 0) { 
            Mat4 rotX180 = Mat4::identity();
            rotX180.m[5] = -1.0f; rotX180.m[10] = -1.0f; 
            return rotX180;
        }
        return Mat4::identity(); 
    }
    if (rotation_axis.length() < 1e-6) return Mat4::identity(); // Should be caught by above, but safety.
    rotation_axis = rotation_axis.normalize();
    float c = std::cos(rotation_angle);
    float s = std::sin(rotation_angle);
    float t = 1.0f - c;
    float x = rotation_axis.x, y = rotation_axis.y, z = rotation_axis.z;
    rotation_matrix.m[0] = t*x*x + c;   rotation_matrix.m[4] = t*x*y - s*z; rotation_matrix.m[8] = t*x*z + s*y;
    rotation_matrix.m[1] = t*x*y + s*z; rotation_matrix.m[5] = t*y*y + c;   rotation_matrix.m[9] = t*y*z - s*x;
    rotation_matrix.m[2] = t*x*z - s*y; rotation_matrix.m[6] = t*y*z + s*x; rotation_matrix.m[10] = t*z*z + c;
    return rotation_matrix;
}

// Helper to draw a single cylinder given its complete model matrix
// (Internal helper for render_frame's bond drawing loop)
void draw_one_cylinder_internal(const Mat4& model_matrix_bond) {
    glUniformMatrix4fv(u_model_matrix_loc, 1, GL_FALSE, model_matrix_bond.m);

    // Calculate and set normal matrix
    Mat4 model_inv_bond = model_matrix_bond.affineInverse(); // Potential performance consideration for many calls
    Mat4 normal_matrix_m4_bond = model_inv_bond.transpose();
    Mat3 normal_matrix_m3_bond = normal_matrix_m4_bond.toMat3();
    glUniformMatrix3fv(u_normal_matrix_loc, 1, GL_FALSE, normal_matrix_m3_bond.m);

    glDrawElements(GL_TRIANGLES, cylinder_index_count, GL_UNSIGNED_INT, 0);
}

void render_frame() {
    if (!gl_context || !shader_program) return;

    // Get current time for auto-rotation
    double current_time = emscripten_get_now() / 1000.0; // Convert to seconds
    if (last_frame_time == 0.0) {
        last_frame_time = current_time;
    }
    double delta_time = current_time - last_frame_time;
    last_frame_time = current_time;

    // Handle auto-rotation
    if (auto_rotate_enabled && !mouse_dragging) {
        camera_angle_y += auto_rotate_speed * delta_time;
        // Keep angle in reasonable range
        if (camera_angle_y > 2.0f * PI) {
            camera_angle_y -= 2.0f * PI;
        }
    }

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);

    // Calculate view matrix based on camera angles and distance
    // Rotation around Y, then X, then translate out by distance
    Mat4 rotY = Mat4::identity(); // Need Mat4::rotateY for full orbit
    Mat4 rotX = Mat4::identity(); // Need Mat4::rotateX for full orbit
    
    // Simplified rotation for now: compose Z rotation for angle_y, and we need X rotation for angle_x
    // This is a placeholder. A proper orbit camera needs more robust rotation.
    // For a simple orbit, we often calculate eye position based on spherical coordinates.
    float eye_x = camera_distance * std::sin(camera_angle_y) * std::cos(camera_angle_x);
    float eye_y = camera_distance * std::sin(camera_angle_x);
    float eye_z = camera_distance * std::cos(camera_angle_y) * std::cos(camera_angle_x);

    view_matrix = Mat4::lookAt(Vec3(eye_x, eye_y, eye_z), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(u_view_matrix_loc, 1, GL_FALSE, view_matrix.m);
    glUniformMatrix4fv(u_projection_matrix_loc, 1, GL_FALSE, projection_matrix.m);

    // Draw Atoms
    glBindVertexArray(sphere_vao);
    for (const auto& atom : current_molecule.atoms) {
        float base_radius = atom.covalent_radius; 
        if (current_representation == Representation::SpaceFill) {
            base_radius = atom.vdw_radius;
        } else if (current_representation == Representation::Licorice) {
            base_radius = atom.covalent_radius * 0.25f; // Licorice atoms are small
        }
        float display_radius = base_radius * g_atom_display_scale_factor;

        // Ensure display_radius is not zero or negative to avoid scaling issues
        if (display_radius <= 0.0f && current_representation != Representation::Licorice) display_radius = 0.01f; 
        else if (display_radius <= 0.0f && current_representation == Representation::Licorice) {
            // For licorice, if atoms are meant to be invisible, we could skip drawing them.
            // For now, let's ensure a tiny non-zero radius if scaling makes it zero.
            // Or, we could have a separate flag/check to not draw atoms in Licorice mode.
            // Let's allow zero radius for licorice for now, effectively making atoms invisible.
        }

        if (display_radius > 0.0f) { // Only draw if radius is positive
            Mat4 model_matrix_atom = Mat4::translate(Vec3(atom.x, atom.y, atom.z)) * Mat4::scale(Vec3(display_radius, display_radius, display_radius));
            glUniformMatrix4fv(u_model_matrix_loc, 1, GL_FALSE, model_matrix_atom.m);
            Mat4 model_inv_atom = model_matrix_atom.affineInverse();
            Mat4 normal_matrix_m4_atom = model_inv_atom.transpose();
            Mat3 normal_matrix_m3_atom = normal_matrix_m4_atom.toMat3();
            glUniformMatrix3fv(u_normal_matrix_loc, 1, GL_FALSE, normal_matrix_m3_atom.m);
            glUniform4f(u_color_loc, atom.color.x, atom.color.y, atom.color.z, 1.0f);
            glDrawElements(GL_TRIANGLES, sphere_index_count, GL_UNSIGNED_INT, 0);
        }
    }
    glBindVertexArray(0);

    // Draw Bonds
    if (current_representation != Representation::SpaceFill && !current_molecule.bonds.empty() && cylinder_vao != 0) {
        glBindVertexArray(cylinder_vao);
        glUniform4f(u_color_loc, bond_color.x, bond_color.y, bond_color.z, 1.0f); 

        for (const auto& bond : current_molecule.bonds) {
            if (bond.atom1_idx >= current_molecule.atoms.size() || bond.atom2_idx >= current_molecule.atoms.size()) {
                std::cerr << "Error: Invalid atom index in bond." << std::endl;
                continue;
            }
            const Atom& atom1 = current_molecule.atoms[bond.atom1_idx];
            const Atom& atom2 = current_molecule.atoms[bond.atom2_idx];

            Vec3 p1(atom1.x, atom1.y, atom1.z);
            Vec3 p2(atom2.x, atom2.y, atom2.z);
            
            float r1_shorten, r2_shorten;
            if (current_representation == Representation::Licorice) {
                r1_shorten = (atom1.covalent_radius * 0.25f) * g_atom_display_scale_factor;
                r2_shorten = (atom2.covalent_radius * 0.25f) * g_atom_display_scale_factor;
                if ( (atom1.covalent_radius * 0.25f * g_atom_display_scale_factor) <= 0.0f) r1_shorten = 0.0f;
                if ( (atom2.covalent_radius * 0.25f * g_atom_display_scale_factor) <= 0.0f) r2_shorten = 0.0f;
            } else { // BallAndStick
                r1_shorten = atom1.covalent_radius * g_atom_display_scale_factor;
                r2_shorten = atom2.covalent_radius * g_atom_display_scale_factor;
            }

            Vec3 bond_vector = p2 - p1;
            float distance_centers = bond_vector.length();

            if (distance_centers < 1e-5) continue; 
            Vec3 bond_direction = bond_vector.normalize();

            float cylinder_actual_length = distance_centers - r1_shorten - r2_shorten;

            if (cylinder_actual_length <= 0.001f) continue; 

            Vec3 cylinder_midpoint = p1 + bond_direction * (r1_shorten + cylinder_actual_length / 2.0f);
            
            Mat4 translation_to_midpoint = Mat4::translate(cylinder_midpoint);
            Mat4 rotation_to_align = align_yaxis_to_vector(bond_direction);
            Mat4 base_transform = translation_to_midpoint * rotation_to_align;

            if (bond.order == 2) { // Double bond
                float double_cyl_eff_radius = bond_radius_scale * DOUBLE_BOND_CYLINDER_RADIUS_SCALE;
                float double_offset_dist = bond_radius_scale * DOUBLE_BOND_OFFSET_FACTOR;

                Mat4 scale_double = Mat4::scale(Vec3(double_cyl_eff_radius, cylinder_actual_length, double_cyl_eff_radius));
                
                // Cylinder 1 (offset in local +X before rotation)
                Mat4 model_dbl1 = base_transform * Mat4::translate(Vec3(double_offset_dist, 0, 0)) * scale_double;
                draw_one_cylinder_internal(model_dbl1);

                // Cylinder 2 (offset in local -X before rotation)
                Mat4 model_dbl2 = base_transform * Mat4::translate(Vec3(-double_offset_dist, 0, 0)) * scale_double;
                draw_one_cylinder_internal(model_dbl2);

            } else if (bond.order == 3) { // Triple bond
                float triple_cyl_eff_radius = bond_radius_scale * TRIPLE_BOND_CYLINDER_RADIUS_SCALE;
                float triple_offset_dist = bond_radius_scale * TRIPLE_BOND_OFFSET_FACTOR;

                Mat4 scale_triple = Mat4::scale(Vec3(triple_cyl_eff_radius, cylinder_actual_length, triple_cyl_eff_radius));

                // Cylinder 1 (Center - no local offset)
                Mat4 model_tpl1 = base_transform * scale_triple;
                draw_one_cylinder_internal(model_tpl1);
                
                // Cylinder 2 (Offset in local +X before rotation)
                Mat4 model_tpl2 = base_transform * Mat4::translate(Vec3(triple_offset_dist, 0, 0)) * scale_triple;
                draw_one_cylinder_internal(model_tpl2);

                // Cylinder 3 (Offset in local -X before rotation)
                Mat4 model_tpl3 = base_transform * Mat4::translate(Vec3(-triple_offset_dist, 0, 0)) * scale_triple;
                draw_one_cylinder_internal(model_tpl3);

            } else { // Single bond (or any other order defaults to single)
                Mat4 scale_single = Mat4::scale(Vec3(bond_radius_scale, cylinder_actual_length, bond_radius_scale));
                Mat4 model_single = base_transform * scale_single;
                draw_one_cylinder_internal(model_single);
            }
        }
        glBindVertexArray(0);
    }
}

extern "C" {
EMSCRIPTEN_KEEPALIVE
void set_atom_display_scale(float scale) {
    if (scale > 0.0f && scale < 10.0f) { // Basic validation for scale
        g_atom_display_scale_factor = scale;
        std::cout << "C++: Atom display scale set to " << g_atom_display_scale_factor << std::endl;
    } else {
        std::cerr << "C++: Invalid atom display scale value: " << scale << std::endl;
    }
}

EMSCRIPTEN_KEEPALIVE
void set_bond_radius_value(float radius) {
    if (radius > 0.0f) {
        bond_radius_scale = radius;
        std::cout << "C++: Bond radius scale set to " << bond_radius_scale << std::endl;
    } else {
        std::cerr << "C++: Invalid bond radius value: " << radius << std::endl;
    }
}

EMSCRIPTEN_KEEPALIVE
void set_zoom_level(float zoom) {
    if (zoom > 0.0f) {
        // Convert zoom level (1.0 = normal) to camera distance
        // Zoom of 1.0 = distance 5.0, zoom of 0.1 = distance 20.0, zoom of 5.0 = distance 1.0
        float base_distance = 5.0f;
        camera_distance = base_distance / zoom;
        
        // Clamp to existing limits
        camera_distance = std::max(MIN_CAMERA_DISTANCE, std::min(MAX_CAMERA_DISTANCE, camera_distance));
        
        std::cout << "C++: Zoom level set to " << zoom << " (camera distance: " << camera_distance << ")" << std::endl;
    } else {
        std::cerr << "C++: Invalid zoom level: " << zoom << std::endl;
    }
}

EMSCRIPTEN_KEEPALIVE
void set_auto_rotate(int enabled) {
    auto_rotate_enabled = (enabled != 0);
    std::cout << "C++: Auto-rotation " << (auto_rotate_enabled ? "enabled" : "disabled") << std::endl;
}

EMSCRIPTEN_KEEPALIVE
void set_representation(int rep_value) {
    if (rep_value >= 0 && rep_value < 3) { // Basic validation
        current_representation = static_cast<Representation>(rep_value);
        std::cout << "C++: Representation set to " << rep_value << std::endl;
    } else {
        std::cerr << "C++: Invalid representation value: " << rep_value << std::endl;
    }
}

EMSCRIPTEN_KEEPALIVE
void update_projection_matrix_aspect(int width, int height) {
    if (height == 0) height = 1; // prevent division by zero
    float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    projection_matrix = Mat4::perspective(PI / 3.0f, aspect_ratio, 0.1f, 100.0f);
    
    // Update WebGL viewport to match the new drawing buffer size
    if (gl_context) { // Make sure GL context is available
        // It's good practice to make the context current if not sure, but Emscripten usually handles this well within callbacks from JS.
        // emscripten_webgl_make_context_current(gl_context); // Usually not needed if called from JS event that doesn't yield
        glViewport(0, 0, width, height);
        std::cout << "C++: Viewport updated to " << width << "x" << height << std::endl;
    } else {
        std::cerr << "C++: GL context not available during viewport update." << std::endl;
    }

    std::cout << "C++: Projection matrix updated for aspect ratio: " << aspect_ratio << std::endl;
}

EMSCRIPTEN_KEEPALIVE
const char* get_current_molecule_name() {
    // Ensure the string isn't empty to avoid issues with c_str() on a potentially null-internal buffer for some std::string impls.
    if (current_molecule.name.empty()) return "N/A";
    return current_molecule.name.c_str();
}

EMSCRIPTEN_KEEPALIVE
const char* get_current_molecule_formula() {
    if (current_molecule.formula.empty()) return "N/A";
    return current_molecule.formula.c_str();
}
} 