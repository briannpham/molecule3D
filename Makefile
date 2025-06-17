# Molecular Visualization Makefile
# Builds the C++ molecular visualization application to WebAssembly

# Compiler and tools
EMCC = emcc
PYTHON = python3

# Directories
SRC_DIR = src
BUILD_DIR = build
DIST_DIR = dist

# Source files
SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/molecule.cpp \
          $(SRC_DIR)/geometry.cpp \
          $(SRC_DIR)/shader.cpp \
          $(SRC_DIR)/input.cpp \
          $(SRC_DIR)/renderer.cpp \
          $(SRC_DIR)/parser.cpp

# Output files
OUTPUT_JS = $(SRC_DIR)/main.js
OUTPUT_WASM = $(SRC_DIR)/main.wasm

# Common Emscripten flags
EMCC_FLAGS = -s USE_WEBGL2=1 \
             -s FULL_ES3=1 \
             -s EXPORTED_FUNCTIONS="['_main']" \
             -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
             -s ALLOW_MEMORY_GROWTH=1

# Development flags
DEV_FLAGS = $(EMCC_FLAGS) \
            -g \
            -O0 \
            -s ASSERTIONS=1 \
            -s SAFE_HEAP=1 \
            --source-map-base http://localhost:8000/src/

# Production flags
PROD_FLAGS = $(EMCC_FLAGS) \
             -O2 \
             --closure 1

# Release flags (maximum optimization)
RELEASE_FLAGS = $(EMCC_FLAGS) \
                -O3 \
                -flto \
                --closure 1 \
                -s ELIMINATE_DUPLICATE_FUNCTIONS=1

# Default target
.PHONY: all
all: production

# Development build (fast compilation, debugging enabled)
.PHONY: dev development
dev development: $(OUTPUT_JS)
	@echo "Development build complete!"
	@echo "Files generated:"
	@echo "  - $(OUTPUT_JS)"
	@echo "  - $(OUTPUT_WASM)"
	@echo "Run 'make serve' to start development server"

$(OUTPUT_JS): $(SOURCES)
	@echo "Building development version..."
	$(EMCC) $(SOURCES) -o $(OUTPUT_JS) $(DEV_FLAGS)

# Production build (optimized for performance)
.PHONY: prod production
prod production:
	@echo "Building production version..."
	$(EMCC) $(SOURCES) -o $(OUTPUT_JS) $(PROD_FLAGS)
	@echo "Production build complete!"
	@echo "Files generated:"
	@echo "  - $(OUTPUT_JS)"
	@echo "  - $(OUTPUT_WASM)"

# Release build (maximum optimization)
.PHONY: release
release:
	@echo "Building release version..."
	$(EMCC) $(SOURCES) -o $(OUTPUT_JS) $(RELEASE_FLAGS)
	@echo "Release build complete!"
	@echo "Files generated:"
	@echo "  - $(OUTPUT_JS)"
	@echo "  - $(OUTPUT_WASM)"

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OUTPUT_JS) $(OUTPUT_WASM)
	rm -f $(SRC_DIR)/*.map
	rm -rf $(BUILD_DIR) $(DIST_DIR)
	@echo "Clean complete!"

# Start development server
.PHONY: serve
serve:
	@echo "Starting development server on http://localhost:8000"
	@echo "Press Ctrl+C to stop the server"
	$(PYTHON) -m http.server 8000

# Start development server on different port
.PHONY: serve-alt
serve-alt:
	@echo "Starting development server on http://localhost:3000"
	@echo "Press Ctrl+C to stop the server"
	$(PYTHON) -m http.server 3000

# Build and serve (development workflow)
.PHONY: dev-serve
dev-serve: development serve

# Check if Emscripten is available
.PHONY: check-env
check-env:
	@echo "Checking Emscripten environment..."
	@which $(EMCC) > /dev/null || (echo "Error: emcc not found. Please activate Emscripten environment with 'source ./emsdk/emsdk_env.sh'" && exit 1)
	@$(EMCC) --version
	@echo "Environment check passed!"

# Show file sizes
.PHONY: size
size:
	@echo "File sizes:"
	@ls -lh $(OUTPUT_JS) $(OUTPUT_WASM) 2>/dev/null || echo "No build files found. Run 'make' first."

# Show build info
.PHONY: info
info:
	@echo "Molecular Visualization Build System"
	@echo "===================================="
	@echo "Source files:"
	@for src in $(SOURCES); do echo "  - $$src"; done
	@echo ""
	@echo "Available targets:"
	@echo "  make dev        - Development build (fast, with debugging)"
	@echo "  make production - Production build (optimized)"
	@echo "  make release    - Release build (maximum optimization)"
	@echo "  make clean      - Remove build artifacts"
	@echo "  make serve      - Start development server"
	@echo "  make dev-serve  - Build and serve"
	@echo "  make check-env  - Check Emscripten environment"
	@echo "  make size       - Show file sizes"
	@echo "  make help       - Show this help"

# Help target
.PHONY: help
help: info

# Create distribution package
.PHONY: dist
dist: release
	@echo "Creating distribution package..."
	mkdir -p $(DIST_DIR)
	cp index.html $(DIST_DIR)/
	cp $(OUTPUT_JS) $(DIST_DIR)/
	cp $(OUTPUT_WASM) $(DIST_DIR)/
	cp -r $(SRC_DIR)/css $(DIST_DIR)/
	cp -r $(SRC_DIR)/js $(DIST_DIR)/
	@echo "Distribution package created in $(DIST_DIR)/"

# Watch for changes (requires inotify-tools on Linux or fswatch on macOS)
.PHONY: watch
watch:
	@echo "Watching for changes... (Press Ctrl+C to stop)"
	@if command -v fswatch >/dev/null 2>&1; then \
		fswatch -o $(SRC_DIR)/*.cpp $(SRC_DIR)/*.h | while read; do \
			echo "Changes detected, rebuilding..."; \
			make dev; \
		done; \
	elif command -v inotifywait >/dev/null 2>&1; then \
		while inotifywait -e modify $(SRC_DIR)/*.cpp $(SRC_DIR)/*.h; do \
			echo "Changes detected, rebuilding..."; \
			make dev; \
		done; \
	else \
		echo "Error: Neither fswatch (macOS) nor inotifywait (Linux) found."; \
		echo "Please install file watching tools for auto-rebuild functionality."; \
		exit 1; \
	fi

# Lint/format code (if clang-format is available)
.PHONY: format
format:
	@if command -v clang-format >/dev/null 2>&1; then \
		echo "Formatting C++ code..."; \
		clang-format -i $(SRC_DIR)/*.cpp $(SRC_DIR)/*.h; \
		echo "Code formatting complete!"; \
	else \
		echo "clang-format not found. Skipping code formatting."; \
	fi

# Show compilation database for IDEs
.PHONY: compile-commands
compile-commands:
	@echo "Generating compile_commands.json for IDE support..."
	$(EMCC) $(SOURCES) $(DEV_FLAGS) -MJ compile_commands.json
	@echo "compile_commands.json generated!"

# Install dependencies (Emscripten setup)
.PHONY: install-deps
install-deps:
	@echo "Setting up Emscripten..."
	@if [ ! -d "emsdk" ]; then \
		echo "Cloning Emscripten SDK..."; \
		git clone https://github.com/emscripten-core/emsdk.git; \
	fi
	cd emsdk && ./emsdk install latest && ./emsdk activate latest
	@echo "Emscripten setup complete!"
	@echo "Run 'source ./emsdk/emsdk_env.sh' to activate the environment."

# Backup current build
.PHONY: backup
backup:
	@if [ -f "$(OUTPUT_JS)" ] && [ -f "$(OUTPUT_WASM)" ]; then \
		timestamp=$$(date +%Y%m%d_%H%M%S); \
		mkdir -p backups; \
		cp $(OUTPUT_JS) backups/main_$$timestamp.js; \
		cp $(OUTPUT_WASM) backups/main_$$timestamp.wasm; \
		echo "Backup created: backups/main_$$timestamp.*"; \
	else \
		echo "No build files to backup."; \
	fi 