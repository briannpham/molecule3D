#pragma once
#include <emscripten/html5.h>

// Camera and Mouse Interaction State
extern float camera_angle_x;
extern float camera_angle_y;
extern float camera_distance; // Initial distance from origin

// Auto-rotation state
extern bool auto_rotate_enabled;
extern bool manual_interaction_active;
extern float auto_rotate_speed; // Radians per second
extern double last_frame_time;

extern bool mouse_dragging;
extern int last_mouse_x, last_mouse_y;
extern const float MOUSE_SENSITIVITY_ROTATE;
extern const float MOUSE_SENSITIVITY_ZOOM_PIXEL_MODE;
extern const float MIN_CAMERA_DISTANCE;
extern const float MAX_CAMERA_DISTANCE;

// Event callback functions
EM_BOOL mousedown_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
EM_BOOL mouseup_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
EM_BOOL mousemove_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
EM_BOOL wheel_callback(int eventType, const EmscriptenWheelEvent *wheelEvent, void *userData); 