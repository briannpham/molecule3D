// main.cpp
#include <iostream>
#include <vector>
#include <string>
#include <map> // Added for std::map
#include <cmath> // For sin, cos, tan, sqrt, acos
#include <algorithm> // for std::max
#include <emscripten/html5.h>
#include <emscripten.h>
#include <GLES3/gl3.h> // For WebGL 2.0 constants and functions
#include <sstream> // For std::istringstream

// --- Configuration ---
const float PI = 3.1415926535f;

// --- Basic Math Utilities (Vec3, Mat4) ---
struct Vec3 {
    float x, y, z;
    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    static float dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }
    float length() const { return std::sqrt(x * x + y * y + z * z); }
    Vec3 normalize() const { float l = length(); return (l > 0) ? Vec3(x / l, y / l, z / l) : Vec3(0,0,0); }
};

struct Mat3 {
    float m[9]; // Column-major: m[col*3 + row]
    Mat3(float diagonal = 1.0f) {
        for(int i=0; i<9; ++i) m[i] = 0.0f;
        m[0] = m[4] = m[8] = diagonal;
    }
    // (More methods like inverse, transpose can be added if needed for mat3 specifically)
};

struct Mat4 {
    float m[16];
    Mat4(float diagonal = 1.0f) { for (int i = 0; i < 16; ++i) m[i] = 0; m[0] = m[5] = m[10] = m[15] = diagonal; }
    static Mat4 identity() { return Mat4(1.0f); }
    static Mat4 translate(const Vec3& t) { Mat4 r = identity(); r.m[12]=t.x; r.m[13]=t.y; r.m[14]=t.z; return r; }
    static Mat4 scale(const Vec3& s) { Mat4 r = identity(); r.m[0]=s.x; r.m[5]=s.y; r.m[10]=s.z; return r; }
    static Mat4 rotateZ(float angle_rad) {
        Mat4 res = identity();
        float c = std::cos(angle_rad);
        float s = std::sin(angle_rad);
        res.m[0] = c; res.m[1] = s;
        res.m[4] = -s; res.m[5] = c;
        return res;
    }
    Mat4 operator*(const Mat4& other) const {
        Mat4 result(0.0f);
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += m[k * 4 + r] * other.m[c * 4 + k];
                }
                result.m[c * 4 + r] = sum;
            }
        }
        return result;
    }
    static Mat4 perspective(float fov_y_rad, float aspect, float z_near, float z_far) {
        Mat4 res(0.0f);
        float tan_half_fovy = std::tan(fov_y_rad / 2.0f);
        res.m[0] = 1.0f / (aspect * tan_half_fovy);
        res.m[5] = 1.0f / (tan_half_fovy);
        res.m[10] = -(z_far + z_near) / (z_far - z_near);
        res.m[11] = -1.0f;
        res.m[14] = -(2.0f * z_far * z_near) / (z_far - z_near);
        return res;
    }
    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).normalize();
        Vec3 s = Vec3::cross(f, up).normalize();
        Vec3 u = Vec3::cross(s, f);

        Mat4 res = identity();
        res.m[0] = s.x; res.m[4] = s.y; res.m[8] = s.z;
        res.m[1] = u.x; res.m[5] = u.y; res.m[9] = u.z;
        res.m[2] = -f.x; res.m[6] = -f.y; res.m[10] = -f.z;
        res.m[12] = -Vec3::dot(s, eye);
        res.m[13] = -Vec3::dot(u, eye);
        res.m[14] = Vec3::dot(f, eye);
        return res;
    }
    Mat3 toMat3() const {
        Mat3 res(0.0f);
        res.m[0] = m[0]; res.m[1] = m[1]; res.m[2] = m[2];
        res.m[3] = m[4]; res.m[4] = m[5]; res.m[5] = m[6];
        res.m[6] = m[8]; res.m[7] = m[9]; res.m[8] = m[10];
        return res;
    }
    Mat4 affineInverse() const {
        Mat4 inv = Mat4::identity();
        float det = m[0]*(m[5]*m[10] - m[6]*m[9]) - m[1]*(m[4]*m[10] - m[6]*m[8]) + m[2]*(m[4]*m[9] - m[5]*m[8]);
        if (std::abs(det) < 1e-6) return Mat4::identity();
        float invDet = 1.0f / det;

        inv.m[0] = (m[5]*m[10] - m[6]*m[9]) * invDet;
        inv.m[1] = (m[2]*m[9] - m[1]*m[10]) * invDet;
        inv.m[2] = (m[1]*m[6] - m[2]*m[5]) * invDet;
        inv.m[4] = (m[6]*m[8] - m[4]*m[10]) * invDet;
        inv.m[5] = (m[0]*m[10] - m[2]*m[8]) * invDet;
        inv.m[6] = (m[2]*m[4] - m[0]*m[6]) * invDet;
        inv.m[8] = (m[4]*m[9] - m[5]*m[8]) * invDet;
        inv.m[9] = (m[1]*m[8] - m[0]*m[9]) * invDet;
        inv.m[10] = (m[0]*m[5] - m[1]*m[4]) * invDet;

        inv.m[12] = -(m[12]*inv.m[0] + m[13]*inv.m[4] + m[14]*inv.m[8]);
        inv.m[13] = -(m[12]*inv.m[1] + m[13]*inv.m[5] + m[14]*inv.m[9]);
        inv.m[14] = -(m[12]*inv.m[2] + m[13]*inv.m[6] + m[14]*inv.m[10]);
        return inv;
    }
    Mat4 transpose() const {
        Mat4 res(0.0f);
        for(int i=0; i<4; ++i) for(int j=0; j<4; ++j) res.m[j*4+i] = m[i*4+j];
        return res;
    }
};
// --- End Math Utilities ---

// --- Molecule Data Structures ---
struct Atom {
    float x, y, z;
    std::string element;
    float covalent_radius; // Was 'radius'
    float vdw_radius;      // Van der Waals radius
    Vec3 color;
};

struct Bond {
    size_t atom1_idx;
    size_t atom2_idx;
    int order = 1; // Default to single bond
};

struct Molecule {
    std::vector<Atom> atoms;
    std::vector<Bond> bonds;
    std::string name;    // For molecule name/comment
    std::string formula; // For calculated molecular formula
    void clear() {
        atoms.clear();
        bonds.clear();
        name.clear();
        formula.clear();
    }
    // Could add global VAO/VBO for the whole molecule later
};

// Helper to get some default atom properties
void get_atom_properties(const std::string& element, float& cov_radius, float& vdw_r, Vec3& color) {
    // Covalent radii and colors (existing)
    if (element == "H")      { cov_radius = 0.37f; color = Vec3(0.9f, 0.9f, 0.9f); } // White
    else if (element == "C") { cov_radius = 0.77f; color = Vec3(0.2f, 0.2f, 0.2f); } // Dark Grey/Black
    else if (element == "N") { cov_radius = 0.75f; color = Vec3(0.2f, 0.2f, 0.8f); } // Blue
    else if (element == "O") { cov_radius = 0.73f; color = Vec3(0.8f, 0.1f, 0.1f); } // Red
    else if (element == "S") { cov_radius = 1.03f; color = Vec3(0.8f, 0.8f, 0.1f); } // Yellow
    else if (element == "P") { cov_radius = 1.07f; color = Vec3(0.8f, 0.5f, 0.1f); } // Orange
    else if (element == "F") { cov_radius = 0.71f; color = Vec3(0.1f, 0.8f, 0.1f); } // Green
    else if (element == "Cl"){ cov_radius = 0.99f; color = Vec3(0.1f, 0.9f, 0.1f); } // Light Green
    else                     { cov_radius = 0.6f; color = Vec3(0.8f, 0.1f, 0.8f); } // Default: Magenta

    // Van der Waals radii (approximate, in Angstroms)
    if (element == "H")      { vdw_r = 1.20f; }
    else if (element == "C") { vdw_r = 1.70f; }
    else if (element == "N") { vdw_r = 1.55f; }
    else if (element == "O") { vdw_r = 1.52f; }
    else if (element == "S") { vdw_r = 1.80f; }
    else if (element == "P") { vdw_r = 1.80f; }
    else if (element == "F") { vdw_r = 1.47f; }
    else if (element == "Cl"){ vdw_r = 1.75f; }
    else                     { vdw_r = 1.5f;  } // Default vdW
}

Molecule create_sample_molecule() {
    Molecule water;
    water.name = "Water (Sample)"; // Assign a name
    
    float cov_r_O, vdw_r_O, cov_r_H, vdw_r_H;
    Vec3 color_O, color_H;

    get_atom_properties("O", cov_r_O, vdw_r_O, color_O);
    water.atoms.push_back({0.0f, 0.0f, 0.0f, "O", cov_r_O, vdw_r_O, color_O});

    get_atom_properties("H", cov_r_H, vdw_r_H, color_H);
    water.atoms.push_back({0.757f, 0.586f, 0.0f, "H", cov_r_H, vdw_r_H, color_H});
    water.atoms.push_back({-0.757f, 0.586f, 0.0f, "H", cov_r_H, vdw_r_H, color_H});

    water.bonds.push_back({0, 1, 1}); // Specify order 1
    water.bonds.push_back({0, 2, 1}); // Specify order 1

    // Calculate formula for sample
    std::map<std::string, int> counts;
    for (const auto& atom : water.atoms) {
        counts[atom.element]++;
    }
    std::string formula_str;
    if (counts.count("C")) { formula_str += "C" + (counts["C"] > 1 ? std::to_string(counts["C"]) : ""); counts.erase("C"); }
    if (counts.count("H")) { formula_str += "H" + (counts["H"] > 1 ? std::to_string(counts["H"]) : ""); counts.erase("H"); }
    for (const auto& pair : counts) { formula_str += pair.first + (pair.second > 1 ? std::to_string(pair.second) : ""); }
    water.formula = formula_str;

    std::cout << "Sample water molecule created: " 
              << water.atoms.size() << " atoms, " 
              << water.bonds.size() << " bonds." << std::endl;
    return water;
}
// --- End Molecule Data Structures ---

// --- Sphere Mesh Data ---
std::vector<float> sphere_vertices; // Will store position (x,y,z) and normal (nx,ny,nz)
std::vector<unsigned int> sphere_indices;
GLsizei sphere_index_count = 0;

void create_uv_sphere(float radius, int latitudes, int longitudes) {
    sphere_vertices.clear();
    sphere_indices.clear();

    for (int i = 0; i <= latitudes; ++i) {
        float theta = i * PI / latitudes; // angle from Y axis
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);

        for (int j = 0; j <= longitudes; ++j) {
            float phi = j * 2 * PI / longitudes; // angle around Y axis
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            // Position
            sphere_vertices.push_back(radius * x);
            sphere_vertices.push_back(radius * y);
            sphere_vertices.push_back(radius * z);
            // Normal (for a sphere centered at origin, normal is just the normalized position vector)
            sphere_vertices.push_back(x);
            sphere_vertices.push_back(y);
            sphere_vertices.push_back(z);
        }
    }

    for (int i = 0; i < latitudes; ++i) {
        for (int j = 0; j < longitudes; ++j) {
            int first = (i * (longitudes + 1)) + j;
            int second = first + longitudes + 1;

            sphere_indices.push_back(first);
            sphere_indices.push_back(second);
            sphere_indices.push_back(first + 1);

            sphere_indices.push_back(second);
            sphere_indices.push_back(second + 1);
            sphere_indices.push_back(first + 1);
        }
    }
    sphere_index_count = static_cast<GLsizei>(sphere_indices.size());
    std::cout << "UV Sphere created: " << sphere_vertices.size()/6 << " vertices, " << sphere_index_count/3 << " triangles." << std::endl;
}
// --- End Sphere Mesh Data ---

// --- Cylinder Mesh Data ---
std::vector<float> cylinder_vertices; // Position (x,y,z) and normal (nx,ny,nz)
std::vector<unsigned int> cylinder_indices;
GLsizei cylinder_index_count = 0;

// Creates a cylinder along the Y axis, from y=-0.5 to y=0.5, with radius 1.
void create_cylinder_mesh(float radius, float height, int segments) {
    cylinder_vertices.clear();
    cylinder_indices.clear();

    float half_height = height / 2.0f;

    // Sides
    for (int i = 0; i <= segments; ++i) {
        float angle = static_cast<float>(i) / segments * 2.0f * PI;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);

        // Vertex at bottom of side
        cylinder_vertices.push_back(x); cylinder_vertices.push_back(-half_height); cylinder_vertices.push_back(z);
        cylinder_vertices.push_back(x/radius); cylinder_vertices.push_back(0.0f); cylinder_vertices.push_back(z/radius); // Normal (outward)

        // Vertex at top of side
        cylinder_vertices.push_back(x); cylinder_vertices.push_back(half_height); cylinder_vertices.push_back(z);
        cylinder_vertices.push_back(x/radius); cylinder_vertices.push_back(0.0f); cylinder_vertices.push_back(z/radius); // Normal (outward)
    }

    for (int i = 0; i < segments; ++i) {
        int current_bottom = i * 2;
        int current_top = i * 2 + 1;
        int next_bottom = (i + 1) * 2;
        int next_top = (i + 1) * 2 + 1;

        cylinder_indices.push_back(current_bottom);
        cylinder_indices.push_back(next_bottom);
        cylinder_indices.push_back(current_top);

        cylinder_indices.push_back(current_top);
        cylinder_indices.push_back(next_bottom);
        cylinder_indices.push_back(next_top);
    }

    // Caps (simple triangle fan, could be improved for better normals at edges)
    // Top cap
    int top_center_idx = cylinder_vertices.size() / 6;
    cylinder_vertices.push_back(0.0f); cylinder_vertices.push_back(half_height); cylinder_vertices.push_back(0.0f); // Center vertex
    cylinder_vertices.push_back(0.0f); cylinder_vertices.push_back(1.0f); cylinder_vertices.push_back(0.0f);    // Normal (up)
    for (int i = 0; i < segments; ++i) {
        cylinder_indices.push_back(top_center_idx);
        cylinder_indices.push_back(i * 2 + 1);             // Current top edge vertex
        cylinder_indices.push_back((i + 1) * 2 + 1);         // Next top edge vertex
    }

    // Bottom cap
    int bottom_center_idx = cylinder_vertices.size() / 6;
    cylinder_vertices.push_back(0.0f); cylinder_vertices.push_back(-half_height); cylinder_vertices.push_back(0.0f);
    cylinder_vertices.push_back(0.0f); cylinder_vertices.push_back(-1.0f); cylinder_vertices.push_back(0.0f);
    for (int i = 0; i < segments; ++i) {
        cylinder_indices.push_back(bottom_center_idx);
        cylinder_indices.push_back((i + 1) * 2);        // Next bottom edge vertex (reversed for winding)
        cylinder_indices.push_back(i * 2);            // Current bottom edge vertex
    }

    cylinder_index_count = static_cast<GLsizei>(cylinder_indices.size());
    std::cout << "Cylinder mesh created: " << cylinder_vertices.size()/6 << " vertices, " << cylinder_index_count/3 << " triangles." << std::endl;
}
// --- End Cylinder Mesh Data ---

// --- Representation Settings ---
enum class Representation {
    BallAndStick,
    SpaceFill,
    Licorice
};
Representation current_representation = Representation::BallAndStick;
// --- End Representation Settings ---

// --- Appearance Settings ---
float g_atom_display_scale_factor = 0.65f; // Default atom scale factor
// bond_radius is already a global float, can be modified directly or via a setter.

// Multiple Bond Rendering Parameters
const float DEFAULT_BOND_RADIUS = 0.10f; // Reference for bond_radius if reset functionality is ever added
const float DOUBLE_BOND_CYLINDER_RADIUS_SCALE = 0.5f;  // Each cylinder in a double bond is X% of main bond_radius
const float DOUBLE_BOND_OFFSET_FACTOR = 1.0f;         // Increased offset from 0.5f

const float TRIPLE_BOND_CYLINDER_RADIUS_SCALE = 0.33f; // Each cylinder in a triple bond is X% of main bond_radius
const float TRIPLE_BOND_OFFSET_FACTOR = 1.34f;        // Increased offset from 0.67f (approx doubled)
// --- End Appearance Settings ---

// Global WebGL context, shader program, matrices, and uniform locations
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE gl_context = 0;
GLuint shader_program = 0;
GLint position_attribute_location = -1;
GLint normal_attribute_location = -1; // Added for normals

Mat4 projection_matrix;
Mat4 view_matrix;
// Mat4 model_matrix; // Will be per-atom now

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

// --- Camera and Mouse Interaction State ---
float camera_angle_x = 0.0f;
float camera_angle_y = 0.0f;
float camera_distance = 5.0f; // Initial distance from origin

bool mouse_is_down = false;
double last_mouse_x = 0.0;
double last_mouse_y = 0.0;

const float MOUSE_SENSITIVITY_ROTATE = 0.003f;
const float MOUSE_SENSITIVITY_ZOOM_PIXEL_MODE = 0.005f;
const float MIN_CAMERA_DISTANCE = 1.0f;
const float MAX_CAMERA_DISTANCE = 20.0f;
// --- End Camera and Mouse Interaction State ---

Vec3 bond_color(0.6f, 0.6f, 0.6f); // Default grey for bonds
float bond_radius = 0.10f;          // Default bond radius updated to 0.10f

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

// --- Emscripten Event Callback Functions ---
EM_BOOL mousedown_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    if (mouseEvent->button == 0) { // Left mouse button
        mouse_is_down = true;
        last_mouse_x = mouseEvent->clientX;
        last_mouse_y = mouseEvent->clientY;
    }
    return EM_TRUE; // Consume the event
}

EM_BOOL mouseup_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    if (mouseEvent->button == 0) { // Left mouse button
        mouse_is_down = false;
    }
    return EM_TRUE; // Consume the event
}

EM_BOOL mousemove_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    if (mouse_is_down) {
        double dx = mouseEvent->clientX - last_mouse_x;
        double dy = mouseEvent->clientY - last_mouse_y;

        camera_angle_y -= static_cast<float>(dx) * MOUSE_SENSITIVITY_ROTATE;
        camera_angle_x += static_cast<float>(dy) * MOUSE_SENSITIVITY_ROTATE;

        // Clamp camera_angle_x to avoid flipping
        camera_angle_x = std::max(-PI / 2.0f + 0.01f, std::min(PI / 2.0f - 0.01f, camera_angle_x));

        last_mouse_x = mouseEvent->clientX;
        last_mouse_y = mouseEvent->clientY;
    }
    return EM_TRUE; // Consume the event
}

EM_BOOL wheel_callback(int eventType, const EmscriptenWheelEvent *wheelEvent, void *userData) {
    float scroll_amount = static_cast<float>(wheelEvent->deltaY);

    // Normalize scroll amount based on deltaMode
    // DOM_DELTA_PIXEL = 0x00, DOM_DELTA_LINE = 0x01, DOM_DELTA_PAGE = 0x02
    if (wheelEvent->deltaMode == 0x01) { // Lines
        scroll_amount *= 30; // Estimate: 30 pixels per line
    } else if (wheelEvent->deltaMode == 0x02) { // Pages
        scroll_amount *= 200; // Estimate: 200 pixels per page
    }

    camera_distance += scroll_amount * MOUSE_SENSITIVITY_ZOOM_PIXEL_MODE;
    camera_distance = std::max(MIN_CAMERA_DISTANCE, std::min(MAX_CAMERA_DISTANCE, camera_distance));
    
    return EM_TRUE; // Consume the event to prevent default page scrolling
}
// --- End Emscripten Event Callback Functions ---

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
                float double_cyl_eff_radius = bond_radius * DOUBLE_BOND_CYLINDER_RADIUS_SCALE;
                float double_offset_dist = bond_radius * DOUBLE_BOND_OFFSET_FACTOR;

                Mat4 scale_double = Mat4::scale(Vec3(double_cyl_eff_radius, cylinder_actual_length, double_cyl_eff_radius));
                
                // Cylinder 1 (offset in local +X before rotation)
                Mat4 model_dbl1 = base_transform * Mat4::translate(Vec3(double_offset_dist, 0, 0)) * scale_double;
                draw_one_cylinder_internal(model_dbl1);

                // Cylinder 2 (offset in local -X before rotation)
                Mat4 model_dbl2 = base_transform * Mat4::translate(Vec3(-double_offset_dist, 0, 0)) * scale_double;
                draw_one_cylinder_internal(model_dbl2);

            } else if (bond.order == 3) { // Triple bond
                float triple_cyl_eff_radius = bond_radius * TRIPLE_BOND_CYLINDER_RADIUS_SCALE;
                float triple_offset_dist = bond_radius * TRIPLE_BOND_OFFSET_FACTOR;

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
                Mat4 scale_single = Mat4::scale(Vec3(bond_radius, cylinder_actual_length, bond_radius));
                Mat4 model_single = base_transform * scale_single;
                draw_one_cylinder_internal(model_single);
            }
        }
        glBindVertexArray(0);
    }
}

// --- Helper Functions ---
std::string generate_molecular_formula(const Molecule& mol) {
    if (mol.atoms.empty()) return "N/A";
    std::map<std::string, int> counts;
    for (const auto& atom : mol.atoms) {
        counts[atom.element]++;
    }

    std::string formula_str;
    // Common convention: C, then H, then alphabetical for others
    if (counts.count("C")) {
        formula_str += "C" + (counts["C"] > 1 ? std::to_string(counts["C"]) : "");
        counts.erase("C");
    }
    if (counts.count("H")) {
        formula_str += "H" + (counts["H"] > 1 ? std::to_string(counts["H"]) : "");
        counts.erase("H");
    }
    // Add remaining elements alphabetically by symbol (map iterates alphabetically by key)
    for (const auto& pair : counts) {
        formula_str += pair.first + (pair.second > 1 ? std::to_string(pair.second) : "");
    }
    if (formula_str.empty() && !mol.atoms.empty()) return "Unknown"; // Should not happen if atoms exist
    return formula_str.empty() ? "N/A" : formula_str;
}
// --- End Helper Functions ---

// --- XYZ File Parsing ---
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
    if (radius > 0.0f && radius < 2.0f) { // Basic validation for radius
        bond_radius = radius; // bond_radius is already global
        std::cout << "C++: Bond radius set to " << bond_radius << std::endl;
    } else {
        std::cerr << "C++: Invalid bond radius value: " << radius << std::endl;
    }
}

EMSCRIPTEN_KEEPALIVE
void load_molecule_from_xyz_string(const char* xyz_data_str) {
    current_molecule.clear();
    current_molecule.name = "N/A"; // Default name
    current_molecule.formula = "N/A"; // Default formula
    std::cout << "C++: Attempting to load molecule from XYZ string..." << std::endl;

    std::istringstream stream(xyz_data_str);
    std::string line;
    int num_atoms = 0;
    int line_number = 0;

    try {
        // Line 1: Number of atoms
        if (std::getline(stream, line)) {
            line_number++;
            if (line.empty()) {
                 std::cerr << "XYZ Parse Error (Line " << line_number << "): Number of atoms line is empty." << std::endl; return;
            }
            num_atoms = std::stoi(line);
            if (num_atoms <= 0) {
                std::cerr << "XYZ Parse Error (Line " << line_number << "): Invalid number of atoms: " << num_atoms << std::endl; return;
            }
        } else {
            std::cerr << "XYZ Parse Error: Could not read number of atoms line." << std::endl; return;
        }

        // Line 2: Comment line (potential name)
        if (std::getline(stream, line)) {
            line_number++;
            current_molecule.name = line; // Store the comment line as the name
            // Trim whitespace from name (optional but good)
            current_molecule.name.erase(0, current_molecule.name.find_first_not_of(" \t\n\r\f\v"));
            current_molecule.name.erase(current_molecule.name.find_last_not_of(" \t\n\r\f\v") + 1);
            if(current_molecule.name.empty()) current_molecule.name = "Untitled Molecule";

        } else {
            std::cerr << "XYZ Parse Error: Could not read comment line." << std::endl; return;
        }

        // Subsequent lines: Atom data
        for (int i = 0; i < num_atoms; ++i) {
            if (std::getline(stream, line)) {
                line_number++;
                if (line.empty() && i < num_atoms -1) { // Allow last line to be empty only if all atoms parsed
                    std::cerr << "XYZ Parse Error (Line " << line_number << "): Atom line is empty." << std::endl; return;
                }
                if (line.empty() && i == num_atoms -1) break; // Trailing empty line after all atoms are fine

                std::istringstream atom_line_stream(line);
                std::string element_symbol;
                float x, y, z; // Removed radius_default
                float cov_r_default, vdw_r_default; // Added separate radius variables
                Vec3 color_default;
                
                if (!(atom_line_stream >> element_symbol >> x >> y >> z)) {
                    std::cerr << "XYZ Parse Error (Line " << line_number << "): Could not parse atom data: " << line << std::endl; return;
                }
                get_atom_properties(element_symbol, cov_r_default, vdw_r_default, color_default);
                current_molecule.atoms.push_back({x, y, z, element_symbol, cov_r_default, vdw_r_default, color_default});
            } else {
                std::cerr << "XYZ Parse Error: Unexpected end of file. Expected " << num_atoms << " atoms, got " << i << std::endl; return;
            }
        }
        std::cout << "C++: Successfully loaded " << current_molecule.atoms.size() << " atoms from XYZ string." << std::endl;
        
        // Generate molecular formula
        current_molecule.formula = generate_molecular_formula(current_molecule);
        std::cout << "C++: Molecule Name: " << current_molecule.name << ", Formula: " << current_molecule.formula << std::endl;

    } catch (const std::invalid_argument& ia) {
        std::cerr << "XYZ Parse Error (Line " << line_number << "): Invalid number format - " << ia.what() << " Line content: \"" << line << "\"" << std::endl;
        current_molecule.clear(); // Clear partially loaded molecule on error
    } catch (const std::out_of_range& oor) {
        std::cerr << "XYZ Parse Error (Line " << line_number << "): Number out of range - " << oor.what() << " Line content: \"" << line << "\"" << std::endl;
        current_molecule.clear();
    } catch (const std::exception& e) {
        std::cerr << "XYZ Parse Error (Line " << line_number << "): Generic error - " << e.what() << " Line content: \"" << line << "\"" << std::endl;
        current_molecule.clear();
    } catch (...) {
        std::cerr << "XYZ Parse Error (Line " << line_number << "): Unknown error during parsing. Line content: \"" << line << "\"" << std::endl;
        current_molecule.clear();
        return; // Return on error so we don't try to generate bonds on incomplete data
    }

    // --- Automatic Bond Generation ---
    if (!current_molecule.atoms.empty()) {
        const float BOND_DISTANCE_TOLERANCE_FACTOR = 1.2f; // Allow bonds up to 20% longer than sum of covalent radii
        // A slightly more generous factor for H bonds or slightly longer bonds if desired: 1.3f
        // Or a fixed tolerance: e.g. sum_radii + 0.4 Angstroms

        size_t num_loaded_atoms = current_molecule.atoms.size();
        for (size_t i = 0; i < num_loaded_atoms; ++i) {
            for (size_t j = i + 1; j < num_loaded_atoms; ++j) { // Iterate unique pairs (j > i)
                const Atom& atom_i = current_molecule.atoms[i];
                const Atom& atom_j = current_molecule.atoms[j];

                Vec3 pos_i(atom_i.x, atom_i.y, atom_i.z);
                Vec3 pos_j(atom_j.x, atom_j.y, atom_j.z);

                float distance_sq = (pos_j - pos_i).length() * (pos_j - pos_i).length(); // More efficient to compare squared distances first
                
                float sum_cov_radii = atom_i.covalent_radius + atom_j.covalent_radius;
                float max_bond_dist = sum_cov_radii * BOND_DISTANCE_TOLERANCE_FACTOR;
                float max_bond_dist_sq = max_bond_dist * max_bond_dist;

                // A common lower bound to avoid bonding everything to H if radii are small
                // Adjust if H atoms are not bonding correctly or too aggressively.
                // float min_bond_dist = 0.4f; // Example: don't bond if atoms are too close (e.g. H-H very short)
                // float min_bond_dist_sq = min_bond_dist * min_bond_dist;

                // if (distance_sq <= max_bond_dist_sq && distance_sq >= min_bond_dist_sq) {
                if (distance_sq <= max_bond_dist_sq && distance_sq > 0.0001f) { // check distance_sq is not zero
                     // Ensure the distance isn't *too* small, which might indicate overlapping identical atoms if data is bad
                     // (though XYZ usually doesn't have this problem). The 0.0001f is a small epsilon.
                    current_molecule.bonds.push_back({i, j, 1}); // Default to order 1 for auto-generated bonds
                }
            }
        }
        std::cout << "C++: Automatically generated " << current_molecule.bonds.size() << " bonds." << std::endl;
    }
    // Note: Bonds are not parsed from XYZ. If needed, bond generation logic (e.g., based on distance) would go here.
    // The bond generation logic above *is* the distance based logic.
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
} // extern "C"
// --- End XYZ File Parsing ---

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