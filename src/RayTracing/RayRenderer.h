#pragma once

#include "Scene.h"

#include "Camera.h"

struct Tile
{
    uint32_t x0, y0, x1, y1;
};

class RayRenderer
{
public:
    RayRenderer() {}

    void Render(float* imageBuffer, std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, int accumulateCount);

private:

    glm::vec3 TraceRay(const Ray& ray, int depth);

    std::shared_ptr<Scene> m_Scene;
    std::shared_ptr<Camera> m_Camera;

    glm::vec3 m_SkyLight{ 0.3f, 0.3f, 0.4f };
    // glm::vec3 m_SkyLight{ 0.0f };
    glm::vec3 m_SkyLightDirection = glm::normalize(glm::vec3{ 1.0f, 1.0f, 1.0f });

    const int m_Depth = 20;
    const uint32_t m_tileSize = 32;
};