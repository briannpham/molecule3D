// Molecule information display management
function initializeMoleculeInfo() {
    const moleculeNameSpan = document.getElementById('moleculeNameDisplay');
    const moleculeFormulaSpan = document.getElementById('moleculeFormulaDisplay');

    function updateMoleculeInfoDisplay() {
        if (!Module.ccall || !moleculeNameSpan || !moleculeFormulaSpan) {
            console.warn("Molecule info display elements or ccall not ready.");
            if(moleculeNameSpan) moleculeNameSpan.textContent = "Error";
            if(moleculeFormulaSpan) moleculeFormulaSpan.textContent = "Error";
            return;
        }
        try {
            const name = Module.ccall('get_current_molecule_name', 'string', [], []);
            const formula = Module.ccall('get_current_molecule_formula', 'string', [], []);
            moleculeNameSpan.textContent = name ? name : "N/A";
            moleculeFormulaSpan.textContent = formula ? formula : "N/A";
        } catch (e) {
            Module.printErr("Error fetching molecule info: " + e);
            moleculeNameSpan.textContent = "Error";
            moleculeFormulaSpan.textContent = "Error";
        }
    }

    // Initial update for sample molecule (if any is loaded by default)
    updateMoleculeInfoDisplay(); 

    // Make updateMoleculeInfoDisplay available globally
    window.updateMoleculeInfoDisplay = updateMoleculeInfoDisplay;
} 