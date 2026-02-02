#include "Core/Input.h"
#include <cstring>

bool Input::keys[1024];
bool Input::keysDown[1024];
bool Input::keysUp[1024];
float Input::sensitivity = 2.0f; // Default "Source-like" feel approximation
double Input::lastMouseX = 0;
double Input::lastMouseY = 0;

void Input::Init(GLFWwindow* window) {
    memset(keys, 0, 1024 * sizeof(bool));
    memset(keysDown, 0, 1024 * sizeof(bool));
    memset(keysUp, 0, 1024 * sizeof(bool));
    
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    
    // Raw Input for precision
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Capture mouse
}

bool Input::GetKey(int key) {
    return keys[key];
}

bool Input::GetKeyDown(int key) {
    bool res = keysDown[key];
    keysDown[key] = false; // Consume
    return res;
}

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
            keysDown[key] = true;
        } else if (action == GLFW_RELEASE) {
            keys[key] = false;
            keysUp[key] = true;
        }
    }
}

void Input::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    lastMouseX = xpos;
    lastMouseY = ypos;
}

void Input::SetMouseSensitivity(float sens) {
    sensitivity = sens;
}

float Input::GetMouseSensitivity() {
    return sensitivity;
}

void Input::GetMousePosition(double& x, double& y) {
    x = lastMouseX;
    y = lastMouseY;
}
