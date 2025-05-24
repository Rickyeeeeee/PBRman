#pragma once

#include "Geometry.h"

class Camera
{
public:
    Camera(const glm::vec3& position, const glm::vec3& front, float width, float height, float focal=1.0f)
        : m_Position(position), m_Width(width), m_Height(height), m_Focal(focal)
    {
        m_Front = glm::normalize(front);
        m_Right = glm::normalize(glm::cross(m_Front, m_YAxis));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }

    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetViewDir() const { return m_Front; }
    float GetWidth() const { return m_Width; }
    float GetHeight() const { return m_Height; }
    float GetAspectRatio() const { return m_Height / m_Width; }
    float GetFocal() const { return m_Focal; }

    Ray GetCameraRay(float px, float py) const 
    {
        Ray ray{};
        ray.Direction = glm::vec3(
            px - m_Width / 2.0f, 
            - (py - m_Height / 2.0f), 
            -m_Focal);
        ray.Normalize();
        ray.ApplyRotate(m_Right, m_Up, -m_Front);
        ray.ApplyOffset(m_Position);

        return ray;
    }

private:
    glm::vec3 m_Position;
    glm::vec3 m_Front;

    glm::vec3 m_Right;
    glm::vec3 m_Up;

    float m_Width;
    float m_Height;
    float m_Focal;

    const glm::vec3 m_YAxis{ 0.0f, 1.0f, 0.0f };
};