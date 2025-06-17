#include "parser.h"
#include "renderer.h"
#include <iostream>
#include <sstream>

extern "C" {
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
} 