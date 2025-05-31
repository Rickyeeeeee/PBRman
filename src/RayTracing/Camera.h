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

    // Rotate the camera around the focus point 
    // The focus point is the intersection of the camera's view direction and the xz plane
    // Rotate the camera position and then update the view direction accordingly
    void Rotate(float angleX, float angleY)
    {
        m_RotationVelocity += glm::vec2(angleX, angleY);
    }

    // Offset the camera position with mouse dragging
    void Translate(float offsetX, float offsetY)
    {
        m_TranslationVelocity += glm::vec2(offsetX, offsetY); // accumulate input
    }
    

    void Zoom(float offset)
    {
        m_ZoomVelocity += offset; // Accumulate input
    }

    void Update(float deltaTime)
    {
        // --- Zoom ---
        if (std::abs(m_ZoomVelocity) > 1e-4f)
        {
            float smoothedZoom = m_ZoomVelocity * deltaTime * m_Smoothness;
            m_Position += m_Front * smoothedZoom;
            if (m_EnableSmoothing)
                m_ZoomVelocity *= std::exp(-m_Smoothness * deltaTime);
            else
                m_ZoomVelocity = 0.0f;
        }

        // --- Translation ---
        if (glm::length(m_TranslationVelocity) > 1e-4f)
        {
            glm::vec2 smoothedTranslation = m_TranslationVelocity * deltaTime * m_Smoothness;
            m_Position += m_Right * smoothedTranslation.x + m_Up * smoothedTranslation.y;
            if(m_EnableSmoothing)
                m_TranslationVelocity *= std::exp(-m_Smoothness * deltaTime);
            else
                m_TranslationVelocity = { 0.0f, 0.0f };
        }

         // --- Rotation ---
        if (glm::length(m_RotationVelocity) > 1e-4f)
        {
            glm::vec2 smoothedRotation = m_RotationVelocity * deltaTime * m_Smoothness;

            glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(smoothedRotation.x), m_Right);
            glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(smoothedRotation.y), m_YAxis);

            // Rotate camera position
            m_Position = rotationX * rotationY * glm::vec4(m_Position, 1.0f);
            m_Front = glm::normalize(rotationX * rotationY * glm::vec4(m_Front, 0.0f));

            m_Right = glm::normalize(glm::cross(m_Front, m_YAxis));
            m_Up = glm::normalize(glm::cross(m_Right, m_Front));

            if (m_EnableSmoothing)
                m_RotationVelocity *= std::exp(-m_Smoothness * deltaTime);
            else
                m_RotationVelocity = { 0.0f, 0.0f };
        }
    }

    glm::mat4 GetViewProjection() const
    {
        glm::mat4 view = glm::lookAtRH(m_Position, m_Position + m_Front, m_Up);
        // Fron right-handed to left-handed, depth range is in [0.0, 1.0f]
        glm::mat4 proj = glm::perspectiveRH_ZO(1.0f, m_Width / m_Height, 0.01f, 1000.0f);
        return proj * view;
    }

    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetViewDir() const { return m_Front; }
    float GetWidth() const { return m_Width; }
    float GetHeight() const { return m_Height; }
    float GetAspectRatio() const { return m_Height / m_Width; }
    float GetFocal() const { return m_Focal; }

    Ray GetCameraRay(float px, float py) const 
    {
        // Normalize pixel coordinates to [-1, 1]
        float ndcX = (px / m_Width) * 2.0f - 1.0f;
        float ndcY = 1.0f - (py / m_Height) * 2.0f; // Flip Y

        // Assume vertical FoV of 1 radian for simplicity
        float tanFovY = tan(0.5f); // or adjust based on your real FoV
        float aspect = m_Width / m_Height;

        glm::vec3 rayDirCameraSpace = glm::normalize(glm::vec3(
            ndcX * aspect * tanFovY,
            ndcY * tanFovY,
            -1.0f // camera looks down -Z in camera space
        ));

        // Transform to world space
        glm::vec3 rayDirWorld =
            rayDirCameraSpace.x * m_Right +
            rayDirCameraSpace.y * m_Up +
            rayDirCameraSpace.z * -m_Front;

        rayDirWorld = glm::normalize(rayDirWorld);

        Ray ray{};
        ray.Direction = rayDirWorld;
        ray.Origin = m_Position;
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

    bool m_EnableSmoothing = false;
    float m_ZoomVelocity = 0.0f;
    float m_CurrentOffset = 0.0f;
    float m_Smoothness = 8.0f; // Higher = snappier, lower = smoother
    glm::vec2 m_TranslationVelocity{0.0f, 0.0f}; // offsetX, offsetY in camera space
    glm::vec2 m_RotationVelocity{0.0f, 0.0f}; // angleX, angleY in degrees



    const glm::vec3 m_YAxis{ 0.0f, 1.0f, 0.0f };
};