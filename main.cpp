// main.cpp
#include <iostream>
#include <vector>
#include <string>
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
    float radius;
    Vec3 color; // Added color
};

struct Bond {
    size_t atom1_idx;
    size_t atom2_idx;
};

struct Molecule {
    std::vector<Atom> atoms;
    std::vector<Bond> bonds;
    void clear() { atoms.clear(); bonds.clear(); }
    // Could add global VAO/VBO for the whole molecule later
};

// Helper to get some default atom properties
void get_atom_properties(const std::string& element, float& radius, Vec3& color) {
    // Very basic properties - can be expanded significantly
    if (element == "H")      { radius = 0.37f; color = Vec3(0.9f, 0.9f, 0.9f); } // White
    else if (element == "C") { radius = 0.77f; color = Vec3(0.2f, 0.2f, 0.2f); } // Dark Grey/Black
    else if (element == "N") { radius = 0.75f; color = Vec3(0.2f, 0.2f, 0.8f); } // Blue
    else if (element == "O") { radius = 0.73f; color = Vec3(0.8f, 0.1f, 0.1f); } // Red
    else if (element == "S") { radius = 1.03f; color = Vec3(0.8f, 0.8f, 0.1f); } // Yellow
    else if (element == "P") { radius = 1.07f; color = Vec3(0.8f, 0.5f, 0.1f); } // Orange
    else if (element == "F") { radius = 0.71f; color = Vec3(0.1f, 0.8f, 0.1f); } // Green
    else if (element == "Cl"){ radius = 0.99f; color = Vec3(0.1f, 0.9f, 0.1f); } // Light Green
    // ... add more elements as needed
    else                     { radius = 0.6f; color = Vec3(0.8f, 0.1f, 0.8f); } // Default: Magenta
}

Molecule create_sample_molecule() {
    Molecule water;
    water.atoms.push_back({0.0f, 0.0f, 0.0f, "O", 0.73f, Vec3(1.0f, 0.1f, 0.1f)});  // Oxygen - Red
    water.atoms.push_back({0.757f, 0.586f, 0.0f, "H", 0.37f, Vec3(0.9f, 0.9f, 0.9f)}); // Hydrogen - White/Light Grey
    water.atoms.push_back({-0.757f, 0.586f, 0.0f, "H", 0.37f, Vec3(0.9f, 0.9f, 0.9f)});

    water.bonds.push_back({0, 1});
    water.bonds.push_back({0, 2});

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

        camera_angle_y += static_cast<float>(dx) * MOUSE_SENSITIVITY_ROTATE;
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

    glBindVertexArray(sphere_vao);
    for (const auto& atom : current_molecule.atoms) {
        Mat4 model_matrix = Mat4::translate(Vec3(atom.x, atom.y, atom.z)) * Mat4::scale(Vec3(atom.radius, atom.radius, atom.radius));
        glUniformMatrix4fv(u_model_matrix_loc, 1, GL_FALSE, model_matrix.m);
        Mat4 model_inv = model_matrix.affineInverse();
        Mat4 normal_matrix_m4 = model_inv.transpose();
        Mat3 normal_matrix_m3 = normal_matrix_m4.toMat3();
        glUniformMatrix3fv(u_normal_matrix_loc, 1, GL_FALSE, normal_matrix_m3.m);
        glUniform4f(u_color_loc, atom.color.x, atom.color.y, atom.color.z, 1.0f);
        glDrawElements(GL_TRIANGLES, sphere_index_count, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

// --- XYZ File Parsing ---
extern "C" {
EMSCRIPTEN_KEEPALIVE
void load_molecule_from_xyz_string(const char* xyz_data_str) {
    current_molecule.clear();
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

        // Line 2: Comment line (ignored)
        if (std::getline(stream, line)) {
            line_number++;
            // We don't do anything with the comment line for now
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
                float x, y, z, radius_default;
                Vec3 color_default;
                
                if (!(atom_line_stream >> element_symbol >> x >> y >> z)) {
                    std::cerr << "XYZ Parse Error (Line " << line_number << "): Could not parse atom data: " << line << std::endl; return;
                }
                get_atom_properties(element_symbol, radius_default, color_default);
                current_molecule.atoms.push_back({x, y, z, element_symbol, radius_default, color_default});
            } else {
                std::cerr << "XYZ Parse Error: Unexpected end of file. Expected " << num_atoms << " atoms, got " << i << std::endl; return;
            }
        }
        std::cout << "C++: Successfully loaded " << current_molecule.atoms.size() << " atoms from XYZ string." << std::endl;

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
    }
    // Note: Bonds are not parsed from XYZ. If needed, bond generation logic (e.g., based on distance) would go here.
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
    std::cout << "Main loop started. Molecule should be visible and interactive." << std::endl;
    return 0;
} 