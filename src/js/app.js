// Main application module configuration and initialization
var Module = {
    preRun: [],
    postRun: [],
    print: (function() {
        var element = document.getElementById('output');
        if (element) element.innerHTML = ''; 
        return function(text) {
            if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
            console.log(text);
            if (element) {
                element.innerHTML += text + "\n";
                element.scrollTop = element.scrollHeight; // Auto-scroll
            }
        };
    })(),
    printErr: function(text) {
        if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
        console.error(text);
        var element = document.getElementById('output');
        if (element) {
            element.innerHTML += "<span style='color: var(--danger-color);'>" + text + "</span>\n";
            element.scrollTop = element.scrollHeight; // Auto-scroll
        }
    },
    canvas: (function() {
        var canvas = document.getElementById('canvas');
        canvas.addEventListener("webglcontextlost", function(e) { 
            alert('WebGL context lost. You will need to reload the page.'); 
            e.preventDefault(); 
        }, false);
        return canvas;
    })(),
    setStatus: function(text) {
        if (text) Module.printErr("[STATUS] " + text);
    },
    totalDependencies: 0,
    monitorRunDependencies: function(left) {
        this.totalDependencies = Math.max(this.totalDependencies, left);
        Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies - left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
    },
    onRuntimeInitialized: function() {
        console.log("Emscripten runtime initialized.");
        Module.print("App initialized. Ready to load molecule.");

        // Initialize all components
        initializeMoleculeInfo();
        initializeMoleculeSearch();
        initializeControls();
        initializeCanvas();
        initializeEventListeners();
    }
};

// Global error handler
window.onerror = function(message, source, lineno, colno, error) {
    Module.printErr("JS Global Error: " + message + " at " + source + ":" + lineno);
    return true; // Prevents the browser's default error handling
}; 