#pragma once
#include <vector>
#include <GLES3/gl3.h>

// Sphere Mesh Data
extern std::vector<float> sphere_vertices; // Will store position (x,y,z) and normal (nx,ny,nz)
extern std::vector<unsigned int> sphere_indices;
extern GLsizei sphere_index_count;

// Cylinder Mesh Data
extern std::vector<float> cylinder_vertices; // Position (x,y,z) and normal (nx,ny,nz)
extern std::vector<unsigned int> cylinder_indices;
extern GLsizei cylinder_index_count;

// Functions
void create_uv_sphere(float radius, int latitudes, int longitudes);
void create_cylinder_mesh(float radius, float height, int segments); 