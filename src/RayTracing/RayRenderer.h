#pragma once

#include "Scene.h"

#include "Camera.h"

class RayRenderer
{
public:
    RayRenderer() {}

    void Render(uint32_t* imageBuffer, std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, int accumulateCount);

private:

    glm::vec3 TraceRay(const Ray& ray, int depth);

    std::shared_ptr<Scene> m_Scene;
    std::shared_ptr<Camera> m_Camera;

    glm::vec3 m_SkyLight{ 0.4f, 0.4f, 0.6f };
    glm::vec3 m_SkyLightDirection = glm::normalize(glm::vec3{ 1.0f, 1.0f, 1.0f });

    const int m_Depth = 5;
};