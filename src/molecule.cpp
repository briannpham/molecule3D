#include "molecule.h"
#include <iostream>
#include <algorithm>

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