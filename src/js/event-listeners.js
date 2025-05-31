// Event listeners for molecule loading and file handling
function initializeEventListeners() {
    initializeLibraryLoadingEvents();
    initializeFileLoadingEvents();
}

function call_cpp_load_molecule(xyzText) {
    if (xyzText.trim() === "") {
        Module.printErr("No XYZ data to load.");
        // Clear info if no data
        const moleculeNameSpan = document.getElementById('moleculeNameDisplay');
        const moleculeFormulaSpan = document.getElementById('moleculeFormulaDisplay');
        if(moleculeNameSpan) moleculeNameSpan.textContent = "N/A";
        if(moleculeFormulaSpan) moleculeFormulaSpan.textContent = "N/A";
        return;
    }
    try {
        Module.ccall(
            'load_molecule_from_xyz_string',
            null, 
            ['string'],
            [xyzText]
        );
        Module.print("JS: Called C++ to load molecule.");
        if (window.updateMoleculeInfoDisplay) {
            window.updateMoleculeInfoDisplay(); // Update info after loading
        }
    } catch (e) {
        console.error("JS: Error calling C++ function:", e);
        Module.printErr("Error calling C++ load function. See console.");
        const moleculeNameSpan = document.getElementById('moleculeNameDisplay');
        const moleculeFormulaSpan = document.getElementById('moleculeFormulaDisplay');
        if(moleculeNameSpan) moleculeNameSpan.textContent = "Error loading name";
        if(moleculeFormulaSpan) moleculeFormulaSpan.textContent = "Error loading formula";
    }
}

function initializeLibraryLoadingEvents() {
    document.getElementById('loadFromLibrary').addEventListener('click', function() {
        const selectedMolecule = document.getElementById('moleculeLibrary').value;
        if (selectedMolecule && MOLECULE_LIBRARY[selectedMolecule]) {
            const moleculeData = MOLECULE_LIBRARY[selectedMolecule];
            document.getElementById('xyzData').value = moleculeData.xyz;
            call_cpp_load_molecule(moleculeData.xyz);
            Module.print(`Loaded ${moleculeData.name} (${moleculeData.formula}) from library.`);
        } else {
            Module.printErr("No molecule selected or molecule not found in library.");
        }
    });

    document.getElementById('loadRandomMolecule').addEventListener('click', function() {
        const moleculeKeys = Object.keys(MOLECULE_LIBRARY);
        if (moleculeKeys.length > 0) {
            const randomKey = moleculeKeys[Math.floor(Math.random() * moleculeKeys.length)];
            const moleculeData = MOLECULE_LIBRARY[randomKey];
            
            // Update the dropdown selection
            document.getElementById('moleculeLibrary').value = randomKey;
            
            // Load the molecule
            document.getElementById('xyzData').value = moleculeData.xyz;
            call_cpp_load_molecule(moleculeData.xyz);
            Module.print(`Randomly loaded ${moleculeData.name} (${moleculeData.formula}) from library.`);
        } else {
            Module.printErr("No molecules available in library.");
        }
    });
}

function initializeFileLoadingEvents() {
    document.getElementById('xyzFilePicker').addEventListener('change', function(event) {
        const file = event.target.files[0];
        if (file) {
            const reader = new FileReader();
            reader.onload = function(e) {
                const fileContent = e.target.result;
                document.getElementById('xyzData').value = fileContent; 
                call_cpp_load_molecule(fileContent);
            };
            reader.onerror = function(e) {
                console.error("File reading error:", e);
                Module.printErr("Error reading file.");
            };
            reader.readAsText(file);
        } else {
            Module.print("No file selected for upload.");
        }
    });
} 