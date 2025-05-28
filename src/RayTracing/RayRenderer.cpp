#include "RayRenderer.h"
#include <future>

void RayRenderer::Render(float* imageBuffer, std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, int accumulateCount)
{

    m_Scene = scene;
    m_Camera = camera;

    std::vector<Tile> tiles;

    auto width = (uint32_t)camera->GetWidth();
    auto height = (uint32_t)camera->GetHeight();

    for (uint32_t i = 0; i < width; i += m_tileSize)
    {
        for (uint32_t j = 0; j < height; j += m_tileSize)
        {
            tiles.push_back(Tile{
                i, j, std::min(i + m_tileSize, width), std::min(j + m_tileSize, height)
            });
        }
    }

    std::vector<std::future<void>> futures;
    
    float count = (float)accumulateCount;
    for (const auto& tile : tiles)
    {
        futures.push_back(std::async(std::launch::async, [&]() {

            for (uint32_t i = tile.x0; i < tile.x1; i++)
            {
                for (uint32_t j = tile.y0; j < tile.y1; j++)
                {
                    auto ray = camera->GetCameraRay((float)i + 0.5f, (float)j + 0.5f);
                    
                    auto L = TraceRay(ray, m_Depth);
                    // Format is 0xAABBGGRR
                    glm::vec3 lastColor;
                    auto pColor = &imageBuffer[(i + j * (uint32_t)camera->GetWidth()) * 3];
                    lastColor.r = pColor[0];
                    lastColor.g = pColor[1];
                    lastColor.b = pColor[2];
                
                    auto color = (lastColor * (count - 1) / count) + (L / count);
                
                    pColor[0] = color.r;
                    pColor[1] = color.g;
                    pColor[2] = color.b;
                }
            }
        }));
    }

    for (auto& fut : futures)
        fut.get();
    // for (uint32_t i = 0; i < camera->GetWidth(); i++) {
    //     for (uint32_t j = 0; j < camera->GetHeight(); j++) {
            
    //     }
    // }
}

glm::vec3 RayRenderer::TraceRay(const Ray& ray, int depth)
{
    if (depth <= 0)
    {
        return glm::vec3{ 0.0f };
    }

    glm::vec3 L{ 0.0f };
    const auto intersect = m_Scene->Intersect(ray);

    if (intersect.HasIntersection)
    {
        Ray scatteredRay;
        glm::vec3 attenuation;
        if (intersect.Material->Scatter(ray, intersect, attenuation, scatteredRay))
        {
            scatteredRay.Normalize();
            if (!(m_Scene->Intersect({ intersect.Position, m_SkyLightDirection }).HasIntersection))
                L += attenuation * glm::vec3{ 0.1f, 0.1f, 0.1f } * glm::clamp(glm::dot(intersect.Normal, m_SkyLightDirection), 0.0f, 1.0f);
            L += attenuation * TraceRay(scatteredRay, depth-1);
        }
    }
    else
    {
        L += m_SkyLight;
    }
    
    return L;
}
