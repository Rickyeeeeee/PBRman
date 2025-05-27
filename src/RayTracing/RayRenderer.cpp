#include "RayRenderer.h"

void RayRenderer::Render(uint32_t* imageBuffer, std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, int accumulateCount)
{

    m_Scene = scene;
    m_Camera = camera;

    float count = (float)accumulateCount;

    for (uint32_t j = 0; j < camera->GetHeight(); j++) {
    for (uint32_t i = 0; i < camera->GetWidth(); i++) {
            
            auto ray = camera->GetCameraRay((float)i + 0.5f, (float)j + 0.5f);
            
            auto L = TraceRay(ray, m_Depth);
            // Format is 0xAABBGGRR
            auto lastColor = imageBuffer[i + j * (uint32_t)camera->GetWidth()];
            glm::vec3 lastL;
            lastL.r = (float)((lastColor >> 0) & 0xFF) / 255.0f;
            lastL.g = (float)((lastColor >> 8) & 0xFF) / 255.0f;
            lastL.b = (float)((lastColor >> 16)  & 0xFF) / 255.0f;

            auto color = (lastL * (count - 1) / count) + (L / count);

            uint32_t colorValue = 
            (uint32_t) (color.r * 255.0f) << 0 | 
            (uint32_t) (color.g * 255.0f) << 8 | 
            (uint32_t) (color.b * 255.0f) << 16 | 
            (uint32_t) 0xFF000000;

            imageBuffer[i + j * (uint32_t)camera->GetWidth()] = colorValue;
        }
    }
}

glm::vec3 RayRenderer::TraceRay(const Ray& ray, int depth)
{
    if (depth <= 0)
    {
        return glm::vec3{ 0.0f };
    }
    depth -= 1;

    glm::vec3 L{ 0.0f };
    const auto intersect = m_Scene->Intersect(ray);

    if (intersect.HasIntersection)
    {
        Ray scatteredRay;
        glm::vec3 attenuation;
        if (intersect.Material->Scatter(ray, intersect, attenuation, scatteredRay))
        {
            scatteredRay.Normalize();
            // if (!(m_Scene->Intersect({ intersect.Position, m_SkyLightDirection }).HasIntersection))
            //     L += attenuation * glm::vec3{ 0.1f, 0.1f, 0.1f } * glm::clamp(glm::dot(intersect.Normal, m_SkyLightDirection), 0.0f, 1.0f);
            L += attenuation * TraceRay(scatteredRay, depth) * glm::clamp(glm::dot(intersect.Normal, scatteredRay.Direction), 0.0f, 1.0f);
        }
    }
    else
    {
        L += m_SkyLight;
    }
    
    return L;
}
