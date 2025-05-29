#pragma once

#include "Primitive.h"
#include <vector>

class Scene
{
public:
    Scene() 
    {
        auto circle = std::make_shared<ShapePrimitive>(
            std::make_shared<Circle>(1.0f),
            std::make_shared<MetalMaterial>(glm::vec3{ 1.0f, 1.0f, 1.0f })
        );
        circle->GetShape().SetTranslation({ 0.0f, 1.0f, 0.0f });
        auto circle2 = std::make_shared<ShapePrimitive>(
            std::make_shared<Circle>(1.0f),
            std::make_shared<LambertianMaterial>(glm::vec3{ 0.2f, 1.0f, 0.2f })
        );
        circle2->GetShape().SetTranslation({ 2.0f, 1.0f, 0.0f });
        auto circle3 = std::make_shared<ShapePrimitive>(
            std::make_shared<Circle>(1.0f),
            std::make_shared<LambertianMaterial>(glm::vec3{ 1.0f, 0.2f, 0.2f })
        );
         circle3->GetShape().SetTranslation({ -2.0f, 1.0f, 0.0f });
        auto circle4 = std::make_shared<ShapePrimitive>(
            std::make_shared<Circle>(1.0f),
            std::make_shared<MetalMaterial>(glm::vec3{ 1.0f, 1.0f, 1.0f })
        );
        circle4->GetShape().SetTranslation(glm::vec3{ 0.0f, 1.0f, 2.3f });
        m_Primitives.push_back(circle);
        m_Primitives.push_back(circle2);
        m_Primitives.push_back(circle3);
        m_Primitives.push_back(circle4);
        circle->GetShape().SetTranslation({ 0.0f, 1.0f, 0.0f });
        auto quad = std::make_shared<ShapePrimitive>(
            std::make_shared<Quad>(10.0f, 10.0f),
            std::make_shared<LambertianMaterial>(glm::vec3{ 0.8f, 0.8f, 0.8f})
        );
        quad->GetShape().SetRotation({ 20.0f, 0.0f, 0.0f });
        quad->GetShape().SetTranslation({ -1.0f, -1.0f, -1.0f });
        m_Primitives.push_back(quad);
        auto tri = std::make_shared<ShapePrimitive>(
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
        );
        tri->GetShape().SetRotation({ -20.0f, 10.0f, 0.0f });
        // m_Primitives.push_back(tri);
    }

    void Intersect(const Ray& ray, SurfaceInteraction* intersect)
    {
        float minDistance = std::numeric_limits<float>::max();
        int minIndex = -1;
        SurfaceInteraction minSurfaceInteraction;
        for (int i = 0; i < m_Primitives.size(); i++)
        {
            auto& primitive = m_Primitives[i];
            SurfaceInteraction si;
            primitive->Intersect(ray, &si);
            auto disVec = ray.Origin - si.Position;
            auto distance2 = glm::dot(disVec, disVec);
            if (si.HasIntersection && distance2 < minDistance)
            {
                minDistance = distance2;
                minIndex = i;
                *intersect = si;
            }
        }

        if (minIndex >= 0)
            return;

        intersect->HasIntersection = false;
        
    }

private:
    std::vector<std::shared_ptr<Primitive>> m_Primitives;
};