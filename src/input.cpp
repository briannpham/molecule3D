#include "input.h"
#include "math.h"
#include <algorithm>

// Camera and Mouse Interaction State
float camera_angle_x = 0.0f;
float camera_angle_y = 0.0f;
float camera_distance = 5.0f; // Initial distance from origin

// Auto-rotation state
bool auto_rotate_enabled = false;
bool manual_interaction_active = false;
float auto_rotate_speed = 0.8f; // Radians per second
double last_frame_time = 0.0;

bool mouse_dragging = false;
int last_mouse_x = 0, last_mouse_y = 0;
const float MOUSE_SENSITIVITY_ROTATE = 0.01f;
const float MOUSE_SENSITIVITY_ZOOM_PIXEL_MODE = 0.005f;
const float MIN_CAMERA_DISTANCE = 1.0f;
const float MAX_CAMERA_DISTANCE = 20.0f;

EM_BOOL mousedown_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    if (mouseEvent->button == 0) { // Left mouse button
        mouse_dragging = true;
        last_mouse_x = mouseEvent->clientX;
        last_mouse_y = mouseEvent->clientY;
    }
    return EM_TRUE; // Consume the event
}

EM_BOOL mouseup_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    if (mouseEvent->button == 0) { // Left mouse button
        mouse_dragging = false;
    }
    return EM_TRUE; // Consume the event
}

EM_BOOL mousemove_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    if (mouse_dragging) {
        double dx = mouseEvent->clientX - last_mouse_x;
        double dy = mouseEvent->clientY - last_mouse_y;

        camera_angle_y -= static_cast<float>(dx) * MOUSE_SENSITIVITY_ROTATE;
        camera_angle_x += static_cast<float>(dy) * MOUSE_SENSITIVITY_ROTATE;

        // Clamp camera_angle_x to avoid flipping
        camera_angle_x = std::max(-PI / 2.0f + 0.01f, std::min(PI / 2.0f - 0.01f, camera_angle_x));

        last_mouse_x = mouseEvent->clientX;
        last_mouse_y = mouseEvent->clientY;
    }
    return EM_TRUE; // Consume the event
}

EM_BOOL wheel_callback(int eventType, const EmscriptenWheelEvent *wheelEvent, void *userData) {
    float scroll_amount = static_cast<float>(wheelEvent->deltaY);

    // Normalize scroll amount based on deltaMode
    // DOM_DELTA_PIXEL = 0x00, DOM_DELTA_LINE = 0x01, DOM_DELTA_PAGE = 0x02
    if (wheelEvent->deltaMode == 0x01) { // Lines
        scroll_amount *= 30; // Estimate: 30 pixels per line
    } else if (wheelEvent->deltaMode == 0x02) { // Pages
        scroll_amount *= 200; // Estimate: 200 pixels per page
    }

    camera_distance += scroll_amount * MOUSE_SENSITIVITY_ZOOM_PIXEL_MODE;
    camera_distance = std::max(MIN_CAMERA_DISTANCE, std::min(MAX_CAMERA_DISTANCE, camera_distance));
    
    return EM_TRUE; // Consume the event to prevent default page scrolling
} 