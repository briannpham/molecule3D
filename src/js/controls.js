// UI Controls management
function initializeControls() {
    initializeRepresentationControl();
    initializeAppearanceControls();
    initializeAutoRotateControl();
}

function initializeRepresentationControl() {
    const representationSelect = document.getElementById('representationSelect');
    if (representationSelect) {
        representationSelect.addEventListener('change', function(event) {
            const repValue = parseInt(event.target.value);
            if (Module.ccall) { // Check if ccall is available
                try {
                    Module.ccall('set_representation', // C function name
                                 null,                 // Return type
                                 ['number'],           // Argument types
                                 [repValue]);          // Arguments
                    Module.print(`Representation set to: ${event.target.options[event.target.selectedIndex].text}`);
                } catch (e) {
                    Module.printErr("Error calling set_representation: " + e);
                }
            } else {
                Module.printErr("Module.ccall is not available. Ensure set_representation is exported.");
            }
        });
    } else {
        Module.printErr("Could not find representationSelect element.");
    }
}

function initializeAppearanceControls() {
    const atomScaleSlider = document.getElementById('atomScaleSlider');
    const atomScaleValueSpan = document.getElementById('atomScaleValue');
    const bondRadiusSlider = document.getElementById('bondRadiusSlider');
    const bondRadiusValueSpan = document.getElementById('bondRadiusValue');
    const zoomSlider = document.getElementById('zoomSlider');
    const zoomValueSpan = document.getElementById('zoomValue');

    if (atomScaleSlider && atomScaleValueSpan) {
        atomScaleSlider.addEventListener('input', function(event) {
            const scale = parseFloat(event.target.value);
            atomScaleValueSpan.textContent = scale.toFixed(2);
            if (Module.ccall) {
                try {
                    Module.ccall('set_atom_display_scale', null, ['number'], [scale]);
                } catch (e) { Module.printErr("Error calling set_atom_display_scale: " + e); }
            }
        });
    } else {
        Module.printErr("Could not find atom scale slider elements.");
    }

    if (bondRadiusSlider && bondRadiusValueSpan) {
        bondRadiusSlider.addEventListener('input', function(event) {
            const radius = parseFloat(event.target.value);
            bondRadiusValueSpan.textContent = radius.toFixed(2);
            if (Module.ccall) {
                try {
                    Module.ccall('set_bond_radius_value', null, ['number'], [radius]);
                } catch (e) { Module.printErr("Error calling set_bond_radius_value: " + e); }
            }
        });
    } else {
        Module.printErr("Could not find bond radius slider elements.");
    }

    if (zoomSlider && zoomValueSpan) {
        zoomSlider.addEventListener('input', function(event) {
            const zoom = parseFloat(event.target.value);
            zoomValueSpan.textContent = zoom.toFixed(2);
            if (Module.ccall) {
                try {
                    Module.ccall('set_zoom_level', null, ['number'], [zoom]);
                } catch (e) { Module.printErr("Error calling set_zoom_level: " + e); }
            }
        });
    } else {
        Module.printErr("Could not find zoom slider elements.");
    }
}

function initializeAutoRotateControl() {
    const autoRotateToggle = document.getElementById('autoRotateToggle');
    let isAutoRotating = false;
    
    if (autoRotateToggle) {
        autoRotateToggle.addEventListener('click', function() {
            isAutoRotating = !isAutoRotating;
            
            if (isAutoRotating) {
                autoRotateToggle.textContent = '⏸️ Stop Auto-Rotate';
                autoRotateToggle.style.backgroundColor = 'var(--danger-color)';
                if (Module.ccall) {
                    try {
                        Module.ccall('set_auto_rotate', null, ['number'], [1]);
                        Module.print("Auto-rotation started");
                    } catch (e) { Module.printErr("Error calling set_auto_rotate: " + e); }
                }
            } else {
                autoRotateToggle.textContent = '🔄 Start Auto-Rotate';
                autoRotateToggle.style.backgroundColor = 'var(--primary-accent-color)';
                if (Module.ccall) {
                    try {
                        Module.ccall('set_auto_rotate', null, ['number'], [0]);
                        Module.print("Auto-rotation stopped");
                    } catch (e) { Module.printErr("Error calling set_auto_rotate: " + e); }
                }
            }
        });
    } else {
        Module.printErr("Could not find auto-rotate toggle button.");
    }
} 