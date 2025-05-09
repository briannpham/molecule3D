# Molecular Viewer Project Plan

This document outlines the steps to build a browser-based molecular visualization engine using C++ and WebAssembly.

## Development Chunks

-   [x] **Chunk 1: Basic Setup**
    -   [x] "Hello World" with C++ and Emscripten.
    -   [x] Compile C++ to WASM and display output in HTML.
    -   [x] Successfully installed Emscripten and compiled/ran the initial code.

-   [x] **Chunk 2: Graphics Setup - Canvas and Basic Drawing**
    -   [x] Modify `index.html` to include a `<canvas>` element for rendering.
    -   [x] Modify `main.cpp` to interface with the canvas using Emscripten's HTML5 API and WebGL.
    -   [x] Draw a simple 2D shape (actually, cleared WebGL canvas) on the canvas from C++.

-   [x] **Chunk 3: 3D Rendering Primitives**
    -   [x] If using SDL2, initialize a 3D rendering context (OpenGL ES via WebGL). If using HTML5 canvas directly, we'll need to manage 3D transformations manually or use a lightweight 3D math library. (We are using WebGL directly for now)
    -   [x] Set up a basic 3D projection and view matrix in C++. (Matrices and shader uniforms implemented)
    -   [x] Create C++ functions to draw 3D primitives:
        -   [x] Spheres (for atoms) (UV Sphere generation implemented)
        -   [ ] Cylinders (for bonds)
    -   [x] Render a single test sphere and a single test cylinder on the canvas. (Rendered test triangle, then multiple spheres for atoms)

-   [x] **Chunk 4: Molecule Data Structures**
    -   [x] Define C++ `struct` or `class` for `Atom` (e.g., `x, y, z` coordinates, element type, color, radius).
    -   [x] Define C++ `struct` or `class` for `Bond` (e.g., indices of two connected atoms).
    -   [x] Define C++ `struct` or `class` for `Molecule` (e.g., a collection of atoms and bonds).
    -   [x] Create a simple C++ function to populate a `Molecule` object with hardcoded data for a small molecule (e.g., water or methane).

-   [x] **Chunk 5: Rendering a Molecule**
    -   [x] Modify the C++ rendering loop to iterate through the atoms in the `Molecule` object.
    -   [x] Render each atom as a sphere at its specified coordinates and with an appropriate color/radius.
    -   [ ] Modify the C++ rendering loop to iterate through the bonds in the `Molecule` object.
    -   [ ] Render each bond as a cylinder connecting the two specified atoms.

-   [x] **Chunk 6: Basic Interaction - Mouse Controls**
    -   [x] Use Emscripten's event handling to capture mouse events.
    -   [x] Implement camera controls in C++ (orbit and zoom).
    -   [x] Update the view matrix in C++ based on mouse input. (Refining sensitivity for smoother experience)
        -   [ ] Orbit rotation: Rotate the camera around the molecule based on mouse drag.
        -   [ ] Zoom: Zoom in/out using the mouse scroll wheel.
        -   [ ] Panning (optional): Move the camera horizontally and vertically.

-   [ ] **Chunk 7: Loading Molecular Data (e.g., XYZ Format)**
    -   [ ] Implement a C++ function to parse a common and simple molecular file format like XYZ.
        -   An XYZ file typically lists the number of atoms, a comment line, and then element symbol with x, y, z coordinates for each atom.
    -   [ ] Create a JavaScript function in `index.html` that allows the user to:
        -   [ ] Upload an XYZ file using an `<input type="file">` element.
        -   [ ] Or paste XYZ data into a `<textarea>`.
    -   [ ] Pass the file content (as a string or ArrayBuffer) from JavaScript to a C++ function using Emscripten bindings (e.g., `ccall` or `cwrap`).
    -   [ ] Update the C++ code to parse this input data, populate the `Molecule` structure, and render it.
    -   [ ] (Optional) Automatically infer bonds based on interatomic distances if the format doesn't explicitly define them.

-   [ ] **Chunk 8: User Interface and Styling**
    -   [ ] Add basic HTML UI elements to `index.html` (e.g., buttons for loading files, display options).
    -   [ ] Style the application for a better user experience.

-   [ ] **Chunk 9: Performance Optimizations & Advanced Rendering**
    -   [ ] Ensure efficient rendering. If not using SDL2/WebGL directly, consider switching to WebGL for hardware acceleration.
    -   [ ] Optimize C++ rendering loops (e.g., minimizing state changes, using vertex buffer objects if using OpenGL/WebGL).
    -   [ ] Consider techniques like instancing for drawing many atoms/bonds efficiently.
    -   [ ] Implement different molecular representations (e.g., ball-and-stick, space-fill, licorice).
    -   [ ] Implement coloring schemes (e.g., by element type - CPK coloring).

-   [ ] **Chunk 10: Further Enhancements (Optional/Future)**
    -   [ ] Support for other file formats (PDB, MOL, SDF).
    -   [ ] Atom/bond picking and selection.
    -   [ ] Displaying labels (atom indices, element symbols, residue names).
    -   [ ] Advanced rendering effects (e.g., ambient occlusion, shadows).
    -   [ ] Integration with JavaScript libraries for UI controls (e.g., sliders for bond/atom sizes). 