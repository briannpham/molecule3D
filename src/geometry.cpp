#include "geometry.h"
#include "math.h"
#include <iostream>

// Sphere Mesh Data
std::vector<float> sphere_vertices; // Will store position (x,y,z) and normal (nx,ny,nz)
std::vector<unsigned int> sphere_indices;
GLsizei sphere_index_count = 0;

// Cylinder Mesh Data
std::vector<float> cylinder_vertices; // Position (x,y,z) and normal (nx,ny,nz)
std::vector<unsigned int> cylinder_indices;
GLsizei cylinder_index_count = 0;

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