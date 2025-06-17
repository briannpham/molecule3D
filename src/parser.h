#pragma once
#include "molecule.h"
#include <emscripten/emscripten.h>

// XYZ File Parsing Functions
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void load_molecule_from_xyz_string(const char* xyz_data_str);
} 