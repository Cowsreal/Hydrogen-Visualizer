#include "Controls.hpp"

Controls::Controls(GLFWwindow* window)
	: m_Window(window)
{
	glfwSetKeyCallback(window, key_callback);
}

bool Controls::KeyLogic(int key) {
	return glfwGetKey(m_Window, key) == GLFW_PRESS;
}

GLFWwindow* Controls::GetWindow()
{
	return m_Window;
}