#include "Camera.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

Camera::Camera(float fov, float aspectRatio, float near, float far, GLFWwindow* window, bool mouseEnabled = false)
	: m_FOV(fov), m_AspectRatio(aspectRatio), m_Near(near), m_Far(far)
{
	m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
	m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
	
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

void Camera::SetFront(const glm::vec3& front) {
	m_Front = front;
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

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= m_MouseSensitivity;
    yoffset *= m_MouseSensitivity;

    m_Yaw   += xoffset;
    m_Pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	front.y = sin(glm::radians(m_Pitch));
	front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(front);
	RecalculateViewMatrix();
}
