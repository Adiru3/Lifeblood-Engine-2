#pragma once
#include <GLFW/glfw3.h>
#include <unordered_map>

class Input {
public:
    static void Init(GLFWwindow* window);
    static bool GetKey(int key);
    static bool GetKeyDown(int key);
    static bool GetKeyUp(int key);
    static void GetMousePosition(double& x, double& y);
    static void SetMouseSensitivity(float sens);
    static float GetMouseSensitivity();
    
    // Internal callbacks
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

private:
    static bool keys[1024];
    static bool keysDown[1024]; // Just pressed this frame
    static bool keysUp[1024];   // Just released this frame
    static float sensitivity;
    static double lastMouseX, lastMouseY;
};
