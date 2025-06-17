#pragma once
#include <vector>
#include <string>
#include <map>
#include "math.h"

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

enum class Representation {
    BallAndStick,
    SpaceFill,
    Licorice
};

// Helper to get some default atom properties
void get_atom_properties(const std::string& element, float& cov_radius, float& vdw_r, Vec3& color);

// Create a sample water molecule
Molecule create_sample_molecule();

// Generate molecular formula from molecule
std::string generate_molecular_formula(const Molecule& mol); 