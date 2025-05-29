#pragma once

#include "Primitive.h"
#include <vector>

class Scene
{
public:
    Scene() 
    {
        auto circle = ShapePrimitive(
            std::make_shared<Circle>(1.0f),
            std::make_shared<MetalMaterial>(glm::vec3{ 1.0f, 1.0f, 1.0f })
        );
        circle.GetShape().SetTranslation({ 0.0f, 1.0f, 0.0f });
        auto circle2 = ShapePrimitive(
            std::make_shared<Circle>(1.0f),
            std::make_shared<LambertianMaterial>(glm::vec3{ 0.2f, 1.0f, 0.2f })
        );
        circle2.GetShape().SetTranslation({ 2.0f, 1.0f, 0.0f });
        auto circle3 = ShapePrimitive(
            std::make_shared<Circle>(1.0f),
            std::make_shared<LambertianMaterial>(glm::vec3{ 1.0f, 0.2f, 0.2f })
        );
         circle3.GetShape().SetTranslation({ -2.0f, 1.0f, 0.0f });
        auto circle4 = ShapePrimitive(
            std::make_shared<Circle>(1.0f),
            std::make_shared<MetalMaterial>(glm::vec3{ 1.0f, 1.0f, 1.0f })
        );
        circle4.GetShape().SetTranslation(glm::vec3{ 0.0f, 1.0f, 2.3f });
        m_Primitives.AddItem(circle);
        m_Primitives.AddItem(circle2);
        m_Primitives.AddItem(circle3);
        m_Primitives.AddItem(circle4);
        auto quad = ShapePrimitive(
            std::make_shared<Quad>(10.0f, 10.0f),
            std::make_shared<LambertianMaterial>(glm::vec3{ 0.8f, 0.8f, 0.8f})
        );
        quad.GetShape().SetRotation({ 20.0f, 0.0f, 0.0f });
        quad.GetShape().SetTranslation({ -1.0f, -1.0f, -1.0f });
        m_Primitives.AddItem(quad);
        auto tri = ShapePrimitive(
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
        tri.GetShape().SetRotation({ -20.0f, 10.0f, 0.0f });
        // m_Primitives.push_back(tri);
    }

    void Intersect(const Ray& ray, SurfaceInteraction* intersect)
    {
        m_Primitives.Intersect(ray, intersect);
    }

private:
    ShapePrimitiveList m_Primitives;
};