#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Controls.hpp"

class Camera 
{
public:
    Camera(float fov, float aspectRatio, float near, float far, GLFWwindow* window);

    void BindControls(Controls* controls);
    const glm::mat4& GetViewMatrix() const;
    const glm::mat4& GetProjectionMatrix() const;
    void ProcessControls();
    glm::vec3 getPosition() { return m_Position; }

    void SetPosition(const glm::vec3& position);
    void SetFOV(float fov);
    void setSpeed(float speed) { m_Speed = speed; }
    void setSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }

private:
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void RecalculateViewMatrix() 
    {
        m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    }

    void RecalculateProjectionMatrix() 
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_Near, m_Far);
    }

    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    float m_FOV;
    float m_AspectRatio;
    float m_Near;
    float m_Far;

    float m_Yaw = -90.0f;                                    // Initial yaw angle (horizontal)
    float m_Pitch = 0.0f;                                   // Initial pitch angle (vertical)
    float m_MouseSensitivity = 0.1f;                             // Mouse sensitivity
    float m_Speed = 0.1f;

    Controls* m_Controls;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;
};
