# Molecular Visualization - Source Components Directory

This directory contains the modular components for the molecular visualization application. The main application interface is located in the root `index.html` file, which references these modular components for better maintainability and organization.

## Directory Structure

```
src/
├── main.cpp                # C++ source code for the molecular visualization engine
├── main.js                 # Compiled JavaScript from Emscripten
├── main.wasm               # Compiled WebAssembly binary
├── css/
│   └── styles.css          # All CSS styles and theming
├── js/
│   ├── app.js              # Main application module and initialization
│   ├── molecule-library.js # Molecule data library
│   ├── molecule-info.js    # Molecule information display management
│   ├── molecule-search.js  # Search functionality for molecules
│   ├── controls.js         # UI controls (sliders, buttons, etc.)
│   ├── canvas.js           # Canvas management and resizing
│   └── event-listeners.js  # Event handling for file loading and interactions
└── README.md               # This documentation file
```

## File Descriptions

### Core Application Files
- **`main.cpp`**: C++ source code containing the molecular visualization engine, OpenGL/WebGL rendering, and mathematical operations.
- **`main.js`**: Emscripten-compiled JavaScript that interfaces between the web frontend and the C++ backend.
- **`main.wasm`**: WebAssembly binary containing the compiled C++ molecular visualization engine.

### CSS
- **`css/styles.css`**: Complete styling including CSS custom properties (variables), responsive design, and component-specific styles.

### JavaScript Modules

- **`js/app.js`**: Core application module containing the Emscripten Module configuration and initialization orchestration.

- **`js/molecule-library.js`**: Contains the `MOLECULE_LIBRARY` constant with all predefined molecule XYZ data.

- **`js/molecule-info.js`**: Manages the display of molecule name and formula information in the sidebar.

- **`js/molecule-search.js`**: Handles the search functionality for filtering molecules in the dropdown.

- **`js/controls.js`**: Manages all UI controls including:
  - Representation selection (Ball & Stick, Space Fill, Licorice)
  - Appearance controls (zoom, atom scale, bond radius)
  - Auto-rotation toggle

- **`js/canvas.js`**: Handles canvas initialization, resizing, and WebGL context management.

- **`js/event-listeners.js`**: Manages event handling for:
  - Loading molecules from library
  - Random molecule loading
  - File upload functionality
  - XYZ data processing

## Benefits of This Structure

1. **Maintainability**: Each module has a single responsibility, making it easier to maintain and debug.

2. **Reusability**: Individual modules can be reused or replaced without affecting others.

3. **Readability**: Code is organized logically, making it easier for developers to understand and contribute.

4. **Performance**: Modules can be loaded asynchronously and cached separately by the browser.

5. **Testing**: Individual modules can be unit tested in isolation.

6. **Collaboration**: Multiple developers can work on different modules simultaneously without conflicts.

7. **Complete Source Control**: All source files (C++, JavaScript, CSS, HTML) are now in one directory.

## Usage

The main application interface is located in the root `../index.html` file, which loads these modular components automatically. All dependencies are loaded in the correct order to ensure proper initialization.

The application maintains the same functionality as the original monolithic version while providing a much cleaner and more maintainable codebase.

## Development Workflow

1. **C++ Development**: Edit `main.cpp` for core visualization logic
2. **Frontend Development**: Edit JavaScript modules in `js/` directory for UI functionality
3. **Styling**: Edit `css/styles.css` for visual appearance
4. **Compilation**: Use Emscripten to compile `main.cpp` to `main.js` and `main.wasm`

## Module Dependencies

The modules are loaded in this order to ensure proper initialization:
1. `molecule-library.js` - Provides data
2. `molecule-info.js` - UI component
3. `molecule-search.js` - UI component  
4. `controls.js` - UI component
5. `canvas.js` - Canvas management
6. `event-listeners.js` - Event handling
7. `app.js` - Main initialization (calls all init functions)
8. `main.js` - Emscripten compiled module (loads `main.wasm`) 