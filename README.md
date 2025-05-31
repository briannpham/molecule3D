# Molecular Visualization Engine

A browser-based molecular visualization engine built with C++ and WebAssembly using Emscripten. This project allows you to visualize molecular structures in 3D with interactive controls for rotation, zooming, and different representation modes.

## Features

- 3D molecular visualization with WebGL rendering
- Support for XYZ file format
- Interactive mouse controls (orbit, zoom)
- Multiple representation modes (Ball-and-Stick, Space-Fill, Licorice)
- Real-time molecular formula calculation
- Modern dark-themed UI
- Modular JavaScript architecture for maintainability

## Prerequisites

- Git
- Python 3.6 or later
- A modern web browser with WebGL support

## Installation

### 1. Install Emscripten SDK (emsdk)

First, clone the Emscripten SDK repository:

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
```

Install and activate the latest version of Emscripten:

```bash
# Fetch the latest version of the emsdk (not needed the first time you clone)
git pull

# Download and install the latest SDK tools
./emsdk install latest

# Make the "latest" SDK "active" for the current user
./emsdk activate latest

# Activate PATH and other environment variables in the current terminal
source ./emsdk_env.sh
```

**Note for Windows users:** Use `emsdk.bat` instead of `./emsdk` and `emsdk_env.bat` instead of `source ./emsdk_env.sh`.

### 2. Verify Installation

Verify that Emscripten is properly installed:

```bash
emcc --version
```

You should see output similar to:
```
emcc (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 3.1.x
```

### 3. Clone This Project

```bash
git clone <your-repository-url>
cd molecular-visualization
```

## Compilation

### Environment Setup

Before compiling, make sure to activate the Emscripten environment in your current terminal session:

```bash
# Navigate to your emsdk directory
cd path/to/emsdk

# Activate the environment
source ./emsdk_env.sh

# Navigate back to your project directory
cd path/to/molecular-visualization
```

### Compile to WebAssembly

Navigate to the `src/` directory and compile the C++ code to WebAssembly:

```bash
cd src
emcc main.cpp -o main.js \
  -s USE_WEBGL2=1 \
  -s FULL_ES3=1 \
  -s EXPORTED_FUNCTIONS="['_main']" \
  -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -O2
```

### Compilation Flags Explained

- `-s USE_WEBGL2=1`: Enable WebGL 2.0 support
- `-s FULL_ES3=1`: Enable full OpenGL ES 3.0 features
- `-s EXPORTED_FUNCTIONS="['_main']"`: Export the main function
- `-s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']"`: Export runtime methods for JavaScript-C++ communication
- `-s ALLOW_MEMORY_GROWTH=1`: Allow dynamic memory allocation
- `-O2`: Optimization level 2 for better performance

### Alternative: Development Build

For development with debugging symbols and faster compilation:

```bash
cd src
emcc main.cpp -o main.js \
  -s USE_WEBGL2=1 \
  -s FULL_ES3=1 \
  -s EXPORTED_FUNCTIONS="['_main']" \
  -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -g \
  -O0
```

## Running the Application

After successful compilation, you'll have the following generated files in the `src/` directory:
- `main.js` - The compiled JavaScript/WebAssembly code
- `main.wasm` - The WebAssembly binary

### Local Development Server

Since the application uses WebAssembly, you need to serve it from a web server (not just open the HTML file directly). You can use Python's built-in server:

```bash
# From the project root directory
# Python 3
python -m http.server 8000

# Python 2 (if you still have it)
python -m SimpleHTTPServer 8000
```

Then open your browser and navigate to:
```
http://localhost:8000
```

The main application is now accessible directly from the root `index.html` file, which loads the modular components from the `src/` directory.

### Alternative Servers

You can also use other local servers:

```bash
# Node.js (if you have npx installed)
npx serve .

# PHP (if you have PHP installed)
php -S localhost:8000
```

## Usage

1. **Loading Molecules**: 
   - Use the file input to upload XYZ format files
   - Or paste XYZ data directly into the text area
   - Click "Load Molecule" to visualize

2. **Controls**:
   - **Mouse drag**: Rotate the molecule
   - **Mouse wheel**: Zoom in/out
   - **Representation dropdown**: Switch between Ball-and-Stick, Space-Fill, and Licorice modes
   - **Atom Scale slider**: Adjust atom sizes
   - **Bond Radius slider**: Adjust bond thickness

3. **Sample Data**: The application loads with a water molecule by default for testing.

## File Structure

```
molecular-visualization/
├── index.html           # Main application interface (loads modular components)
├── src/                 # Source directory with modular components
│   ├── main.cpp         # C++ source code
│   ├── main.js          # Generated JavaScript/WASM (excluded from git)
│   ├── main.wasm        # Generated WebAssembly binary (excluded from git)
│   ├── css/
│   │   └── styles.css   # Application styles
│   ├── js/              # Modular JavaScript components
│   │   ├── app.js       # Main application module
│   │   ├── molecule-library.js
│   │   ├── molecule-info.js
│   │   ├── molecule-search.js
│   │   ├── controls.js
│   │   ├── canvas.js
│   │   └── event-listeners.js
│   └── README.md        # Detailed source documentation
├── planning.md          # Development roadmap
├── README.md            # This file
├── .gitignore           # Git ignore rules
└── emsdk/              # Emscripten SDK (if cloned locally)
```

## Development Notes

- The `main.js` and `main.wasm` files are generated during compilation and should not be committed to version control
- Make sure to activate the Emscripten environment (`source ./emsdk_env.sh`) in each new terminal session before compiling
- For production builds, use `-O2` or `-O3` optimization flags
- The application requires WebGL 2.0 support in the browser

## Architecture

The application follows a modular architecture:

- **C++ Backend** (`main.cpp`): Handles molecular data processing, 3D mathematics, and WebGL rendering
- **JavaScript Frontend**: Modular components for UI, event handling, and application state
- **CSS Styling**: Modern dark theme with CSS custom properties for easy theming
- **HTML Structure**: Clean semantic markup with proper accessibility considerations

## Troubleshooting

### Common Issues

1. **"emcc: command not found"**
   - Make sure you've activated the Emscripten environment: `source ./emsdk_env.sh`

2. **WebAssembly compilation errors**
   - Ensure you're using a recent version of Emscripten
   - Check that all required flags are included in the compilation command

3. **Browser compatibility issues**
   - Ensure your browser supports WebGL 2.0
   - Try using a different browser (Chrome, Firefox, Safari, Edge)

4. **CORS errors when loading locally**
   - Make sure you're serving the files through a web server, not opening the HTML file directly

### Getting Help

If you encounter issues:
1. Check the browser's developer console for error messages
2. Verify that all compilation steps completed successfully
3. Ensure your browser supports WebGL 2.0
