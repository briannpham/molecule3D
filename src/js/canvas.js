// Canvas management and resizing
function initializeCanvas() {
    const mainContent = document.querySelector('.main-content');
    const canvas = Module.canvas;
    
    function resizeCanvas() {
        const dpr = window.devicePixelRatio || 1;
        const newCssWidth = mainContent.clientWidth;
        const newCssHeight = mainContent.clientHeight;

        const newBufferWidth = Math.round(newCssWidth * dpr);
        const newBufferHeight = Math.round(newCssHeight * dpr);

        canvas.width = newBufferWidth;
        canvas.height = newBufferHeight;
        canvas.style.width = newCssWidth + 'px';
        canvas.style.height = newCssHeight + 'px';
        
        // Enhanced logging
        console.log(`[resizeCanvas] Event. CSS Dimensions: ${newCssWidth}w x ${newCssHeight}h. DPR: ${dpr}.`);
        console.log(`[resizeCanvas] Calculated Buffer: ${newBufferWidth}w x ${newBufferHeight}h.`);
        console.log(`[resizeCanvas] Set canvas.width=${canvas.width}, canvas.height=${canvas.height}.`);
        console.log(`[resizeCanvas] Set canvas.style.width='${canvas.style.width}', canvas.style.height='${canvas.style.height}'.`);

        if (Module.ccall) {
            try {
               Module.ccall('update_projection_matrix_aspect', 
                            null, 
                            ['number', 'number'], 
                            [canvas.width, canvas.height]); // Pass the actual buffer width and height
                console.log(`[resizeCanvas] Called C++ update_projection_matrix_aspect(${canvas.width}, ${canvas.height}).`);
            } catch (e) {
                console.warn("[resizeCanvas] Error calling C++ update_projection_matrix_aspect:", e);
            }
        } else {
            console.warn("[resizeCanvas] Module.ccall not available or C++ function update_projection_matrix_aspect not ready.");
        }
    }
    
    // Initial resize, delayed slightly to allow layout to stabilize
    setTimeout(function() {
        console.log("[onRuntimeInitialized] Calling initial resizeCanvas after timeout.");
        resizeCanvas(); 
    }, 100); 

    window.addEventListener('resize', resizeCanvas); // Resize on window change
} 