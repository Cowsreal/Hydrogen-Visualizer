#pragma once

#include <vector>
#include <cmath>

#include "glfw/glfw3.h"

#include "Camera.hpp"

#define PI 3.14159265358979323846

void ChangeStates(bool &s1, bool&s2);
std::vector<std::vector<double>> GenerateGreatCircle(float rotationX, float rotationY, float rotationZ, int n);
std::vector<double> GetColor(std::vector<double> point);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void SetVsync(bool &vsync);