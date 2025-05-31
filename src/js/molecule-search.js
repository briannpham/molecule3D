// Molecule search and library functionality
function initializeMoleculeSearch() {
    const moleculeSearch = document.getElementById('moleculeSearch');
    const moleculeLibrary = document.getElementById('moleculeLibrary');
    const allOptions = Array.from(moleculeLibrary.querySelectorAll('option, optgroup'));
    
    if (moleculeSearch && moleculeLibrary) {
        moleculeSearch.addEventListener('input', function(event) {
            const searchTerm = event.target.value.toLowerCase();
            
            // Reset all options to visible
            allOptions.forEach(element => {
                element.style.display = '';
            });
            
            if (searchTerm) {
                // Automatically expand the dropdown when searching
                moleculeLibrary.size = Math.min(10, moleculeLibrary.options.length);
                moleculeLibrary.style.height = 'auto';
                
                let visibleCount = 0;
                
                // Hide options that don't match
                allOptions.forEach(element => {
                    if (element.tagName === 'OPTION' && element.value) {
                        const optionText = element.textContent.toLowerCase();
                        const moleculeData = MOLECULE_LIBRARY[element.value];
                        const moleculeName = moleculeData ? moleculeData.name.toLowerCase() : '';
                        const moleculeFormula = moleculeData ? moleculeData.formula.toLowerCase() : '';
                        
                        if (!optionText.includes(searchTerm) && 
                            !moleculeName.includes(searchTerm) && 
                            !moleculeFormula.includes(searchTerm)) {
                            element.style.display = 'none';
                        } else {
                            visibleCount++;
                        }
                    }
                });
                
                // Hide empty optgroups
                allOptions.forEach(element => {
                    if (element.tagName === 'OPTGROUP') {
                        const visibleOptions = Array.from(element.querySelectorAll('option')).filter(opt => opt.style.display !== 'none');
                        if (visibleOptions.length === 0) {
                            element.style.display = 'none';
                        }
                    }
                });
                
                // Adjust dropdown size based on visible results
                moleculeLibrary.size = Math.min(Math.max(3, visibleCount + 2), 10);
                
            } else {
                // Reset to normal dropdown when search is cleared
                moleculeLibrary.size = 1;
                moleculeLibrary.style.height = '';
            }
        });
        
        // Also expand dropdown when clicking on search field
        moleculeSearch.addEventListener('focus', function(event) {
            if (event.target.value.trim()) {
                moleculeLibrary.size = Math.min(10, moleculeLibrary.options.length);
            }
        });
        
        // Collapse dropdown when clicking elsewhere
        document.addEventListener('click', function(event) {
            if (!moleculeSearch.contains(event.target) && !moleculeLibrary.contains(event.target)) {
                if (!moleculeSearch.value.trim()) {
                    moleculeLibrary.size = 1;
                    moleculeLibrary.style.height = '';
                }
            }
        });
    }
} 