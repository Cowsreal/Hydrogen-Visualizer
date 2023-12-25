#include "Camera.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

Camera::Camera(float fov, float aspectRatio, float near, float far, GLFWwindow* window)
	: m_FOV(fov), m_AspectRatio(aspectRatio), m_Near(near), m_Far(far)
{
	m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
	m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Set the user pointer to the camera instance
	glfwSetWindowUserPointer(window, this);

	// Set up the mouse callback with the static function
	glfwSetCursorPosCallback(window, mouse_callback);
	RecalculateViewMatrix();
	RecalculateProjectionMatrix();
}

void Camera::SetFOV(float fov)
{
	m_FOV = fov;
	RecalculateProjectionMatrix();
}

void Camera::SetPosition(const glm::vec3& position) {
	m_Position = position;
	RecalculateViewMatrix();
}

void Camera::BindControls(Controls* controls) {
	m_Controls = controls;
}

const glm::mat4& Camera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

const glm::mat4& Camera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

void Camera::ProcessControls()
{
	if (m_Controls->KeyLogic(GLFW_KEY_W))
	{
		m_Position += m_Front * m_Speed;
	}
	if (m_Controls->KeyLogic(GLFW_KEY_S))
	{
		m_Position -= m_Front * m_Speed;
	}
	if (m_Controls->KeyLogic(GLFW_KEY_A))
	{
		m_Position -= glm::normalize(glm::cross(m_Front, m_Up)) * m_Speed;
	}
	if (m_Controls->KeyLogic(GLFW_KEY_D))
	{
		m_Position += glm::normalize(glm::cross(m_Front, m_Up)) * m_Speed;
	}
	if (m_Controls->KeyLogic(GLFW_KEY_SPACE))
	{
		m_Position += m_Up * m_Speed;
	}
	if (m_Controls->KeyLogic(GLFW_KEY_LEFT_SHIFT))
	{
		m_Position -= m_Up * m_Speed;
	}
	/*
	if (m_Controls->KeyLogic(GLFW_KEY_ESCAPE))
	{
		glfwSetInputMode(m_Controls->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(m_Controls->GetWindow(), mouse_callback);
	}
	if (m_Controls->KeyLogic(GLFW_KEY_ENTER))
	{
		glfwSetInputMode(m_Controls->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPosCallback(m_Controls->GetWindow(), NULL);
	}
	*/
	RecalculateViewMatrix();
}

void Camera::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (camera != NULL)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		{
			static float lastX = 0.0f;
			static float lastY = 0.0f;
			static bool firstMouse = true;

			if (firstMouse)
			{
				lastX = xpos;
				lastY = ypos;
				firstMouse = false;
			}

			float xOffset = xpos - lastX;
			float yOffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

			lastX = xpos;
			lastY = ypos;

			xOffset *= camera->m_MouseSensitivity;
			yOffset *= camera->m_MouseSensitivity;

			camera->m_Yaw += xOffset;
			camera->m_Pitch += yOffset;

			// Make sure that when pitch is out of bounds, screen doesn't get flipped
			if (camera->m_Pitch > 89.0f)
				camera->m_Pitch = 89.0f;
			if (camera->m_Pitch < -89.0f)
				camera->m_Pitch = -89.0f;

			// Update Front, Right and Up Vectors using the updated Euler angles
			glm::vec3 front;
			front.x = cos(glm::radians(camera->m_Yaw)) * cos(glm::radians(camera->m_Pitch));
			front.y = sin(glm::radians(camera->m_Pitch));
			front.z = sin(glm::radians(camera->m_Yaw)) * cos(glm::radians(camera->m_Pitch));
			camera->m_Front = glm::normalize(front);
			camera->RecalculateViewMatrix();
		}
		io.WantCaptureMouse = glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED;
	}
}