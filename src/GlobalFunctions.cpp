#include "GlobalFunctions.hpp"
#include <iostream>

void ChangeStates(bool &s1, bool&s2)
{
    s1 = !s1;
	s2 = !s2;
}

std::vector<std::vector<double>> GenerateGreatCircle(float rotationX, float rotationY, float rotationZ, int n) 
{
    std::vector<std::vector<double>> circlePoints;

    // Define the rotation matrix using GLM
    glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), static_cast<float>(rotationX), glm::vec3(1, 0, 0));
    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), static_cast<float>(rotationY), glm::vec3(0, 1, 0));
    glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), static_cast<float>(rotationZ), glm::vec3(0, 0, 1));
    glm::mat4 rotationMatrix = rotZ * rotY * rotX; // Combined rotation matrix

    // Generate points along the base great circle (equator)
    for (int i = 0; i < n; ++i) {
        float t = 2 * PI * i / n; // parameter t along the circle
        glm::vec4 basePoint = glm::vec4(cos(t), sin(t), 0.0f, 1.0f); // Point on the base circle

        // Rotate the point
        glm::vec4 rotatedPoint = rotationMatrix * basePoint;

        // Add the rotated point to the list as a vector of doubles
        circlePoints.push_back({rotatedPoint.x, rotatedPoint.y, rotatedPoint.z});
    }

    return circlePoints;
}

std::vector<double> GetColor(std::vector<double> point)
{
    double x = point[0];
    double y = point[1];
    double z = point[2];

    // Convert x, y, z to hue value
    double hue = atan2(y, x) / (2 * PI);
    if (hue < 0)
    {
        hue += 1.0;
    }

    double saturation = 1.0;
    double value = 1.0;

    double c = 1;
    double w = (1 - std::abs(std::fmod(6 * hue, 2) - 1));
    double m = 0;

    double r, g, b;
    if (hue < 1.0 / 6.0)
    {
        r = c;
        g = w;
        b = 0;
    }
    else if (hue < 2.0 / 6.0)
    {
        r = w;
        g = c;
        b = 0;
    }
    else if (hue < 3.0 / 6.0)
    {
        r = 0;
        g = c;
        b = w;
    }
    else if (hue < 4.0 / 6.0)
    {
        r = 0;
        g = w;
        b = c;
    }
    else if (hue < 5.0 / 6.0)
    {
        r = w;
        g = 0;
        b = c;
    }
    else
    {
        r = c;
        g = 0;
        b = w;
    }

    // Add the same alpha value for all colors
    double alpha = 1.0;

    return {r, g, b, alpha};
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if(glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
    {
        return;
    }
    static float lastX = 1920 / 2.0; // Half of window width
    static float lastY = 1080 / 2.0; // Half of window height
    static bool firstMouse = true;

    if (firstMouse) 
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    camera->ProcessMouseMovement(xoffset, yoffset, GL_TRUE);
}

void SetVsync(bool &vsync)
{
    if (vsync)
    {
        glfwSwapInterval(1);
    }
    else
    {
        glfwSwapInterval(0);
    }
}