#pragma once

#include <GLFW/glfw3.h>
#include <iostream>


class Controls {
public:
	Controls(GLFWwindow* window);
	bool KeyLogic(int key);
	GLFWwindow* GetWindow();

private:
	GLFWwindow* m_Window;
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
	}
};