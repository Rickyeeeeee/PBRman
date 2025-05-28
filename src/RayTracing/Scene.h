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
            std::make_shared<MetalMaterial>(glm::vec3{ 1.0f, 1.0f, 1.0f })
        };
        circle.Shape->SetTranslation({ 0.0f, 1.0f, 0.0f });
        Primitive circle2 {
            std::make_shared<Circle>(1.0f),
            std::make_shared<LambertianMaterial>(glm::vec3{ 0.2f, 1.0f, 0.2f })
        };
        circle2.Shape->SetTranslation({ 2.0f, 1.0f, 0.0f });
        Primitive circle3 {
            std::make_shared<Circle>(1.0f),
            std::make_shared<LambertianMaterial>(glm::vec3{ 1.0f, 0.2f, 0.2f })
        };
        circle3.Shape->SetTranslation({ -2.0f, 1.0f, 0.0f });
        Primitive circle4 {
            std::make_shared<Circle>(1.0f),
            std::make_shared<MetalMaterial>(glm::vec3{ 1.0f, 1.0f, 1.0f })
        };
        circle4.Shape->SetTranslation(glm::vec3{ 0.0f, 1.0f, 2.3f });
        m_Primitives.push_back(circle);
        m_Primitives.push_back(circle2);
        m_Primitives.push_back(circle3);
        m_Primitives.push_back(circle4);
        circle.Shape->SetTranslation({ 0.0f, 1.0f, 0.0f });
        Primitive quad = {
            std::make_shared<Quad>(10.0f, 10.0f),
            std::make_shared<LambertianMaterial>(glm::vec3{ 0.8f, 0.8f, 0.8f})
        };
        quad.Shape->SetRotation({ 20.0f, 0.0f, 0.0f });
        quad.Shape->SetTranslation({ -1.0f, -1.0f, -1.0f });
        m_Primitives.push_back(quad);
        Primitive tri{
            std::make_shared<Triangle>(
                glm::vec3{ 5.0f, 0.0f, 5.0f },
                glm::vec3{ 0.0f, 0.0f, -5.0f },
                glm::vec3{ -5.0f, 0.0f, 5.0f },
                glm::vec3{ 0.0f, 1.0f, 0.0f },
                glm::vec3{ 0.0f, 1.0f, 0.0f },
                glm::vec3{ 0.0f, 1.0f, 0.0f },
                glm::vec2{ 0.0f },
                glm::vec2{ 0.0f },
                glm::vec2{ 0.0f }
            ),
            std::make_shared<LambertianMaterial>(glm::vec3{ 0.8f, 0.8f, 0.8f})
        };
        tri.Shape->SetRotation({ -20.0f, 10.0f, 0.0f });
        m_Primitives.push_back(tri);
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