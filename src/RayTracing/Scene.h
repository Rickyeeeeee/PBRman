#pragma once

#include "Primitive.h"
#include <vector>

class Scene
{
public:
    Scene() 
    {
        Primitive circle {
            std::make_shared<Circle>(1.0f),
            std::make_shared<LambertianMaterial>(glm::vec3{ 1.0f, 0.1f, 0.1f })
        };
        circle.Shape->SetTranslation({ 0.0f, 1.0f, 0.0f });
        Primitive circle2 {
            std::make_shared<Circle>(1.0f),
            std::make_shared<LambertianMaterial>(glm::vec3{ 1.0f, 0.1f, 0.1f })
        };
        circle2.Shape->SetTranslation({ 2.0f, 1.0f, 0.0f });
        m_Primitives.push_back(circle);
        m_Primitives.push_back(circle2);
        m_Primitives.push_back({
            std::make_shared<Quad>(10.0f, 10.0f),
            std::make_shared<LambertianMaterial>(glm::vec3{ 0.8f, 0.8f, 0.8f})
        });
    }

    SurfaceInteraction Intersect(const Ray& ray)
    {
        float minDistance = std::numeric_limits<float>::max();
        int minIndex = -1;
        SurfaceInteraction minSurfaceInteraction;
        for (int i = 0; i < m_Primitives.size(); i++)
        {
            auto& primitive = m_Primitives[i];
            auto si = primitive.Shape->Intersect(ray);
            auto distance = glm::length(ray.Origin - si.Position);
            if (si.HasIntersection && distance < minDistance)
            {
                minDistance = distance;
                minIndex = i;
                si.Material = primitive.Material;
                minSurfaceInteraction = si;
            }
        }

        if (minIndex >= 0)
            return minSurfaceInteraction;
        
        return SurfaceInteraction();
    }

private:
    std::vector<Primitive> m_Primitives;
};