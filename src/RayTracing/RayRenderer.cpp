#include "RayRenderer.h"

const float pi = 3.1415926535897932385;

void RayRenderer::Render(float* imageBuffer, std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, int accumulateCount)
{

    m_Scene = scene;
    m_Camera = camera;

    float count = (float)accumulateCount;

    for (uint32_t i = 0; i < camera->GetWidth(); i++) {
        for (uint32_t j = 0; j < camera->GetHeight(); j++) {
            
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
