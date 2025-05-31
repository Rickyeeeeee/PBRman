#pragma once

#include "Primitive.h"
#include <vector>
#include "Core/Mesh.h"

class Scene
{
public:
    Scene() 
    {
        auto circle = std::make_shared<SimplePrimitive>(
            std::make_shared<Circle>(1.0f),
            // std::make_unique<MetalMaterial>(glm::vec3{ 1.0f, 1.0f, 1.0f })
            std::make_unique<DielectricMaterial>(0.9f)
        );
        circle->SetTransform(
            glm::vec3{ 1.5f },
            glm::vec3{ 0.0f },
            glm::vec3{ 0.0f, 2.0f, 0.0f }
        );
        auto circle2 = std::make_shared<SimplePrimitive>(
            std::make_shared<Circle>(1.0f),
            std::make_unique<LambertianMaterial>(glm::vec3{ 0.2f, 1.0f, 0.2f })
        );
        circle2->SetTransform(
            glm::vec3{ 1.0f },
            glm::vec3{ 0.0f },
            glm::vec3{ 2.0f, 1.0f, 0.0f }
        );
        auto circle3 = std::make_shared<SimplePrimitive>(
            std::make_shared<Circle>(1.0f),
            std::make_unique<LambertianMaterial>(glm::vec3{ 1.0f, 0.2f, 0.2f })
        );
        circle3->SetTransform(
            glm::vec3{ 1.0f },
            glm::vec3{ 0.0f },
            glm::vec3{ -2.0f, 1.0f, 0.0f }
        );
        auto circle4 = std::make_shared<SimplePrimitive>(
            std::make_shared<Circle>(1.0f),
            std::make_unique<LambertianMaterial>(glm::vec3{ 1.0f, 1.0f, 1.0f })
        );
        circle4->SetTransform(
            glm::vec3{ 1.0f },
            glm::vec3{ 0.0f },
            glm::vec3{ 0.0f, 1.0f, 2.3f }
        );
        auto circle5 = std::make_shared<SimplePrimitive>(
            std::make_shared<Circle>(1.0f),
            std::make_unique<MetalMaterial>(glm::vec3{ 1.0f, 0.7, 0.8f }, 0.1f)
        );
        circle5->SetTransform(
            glm::vec3{ 1.0f },
            glm::vec3{ 0.0f },
            glm::vec3{ 0.0f, 1.0f, -2.3f }
        );
        m_Primitives.AddItem(circle);
        m_Primitives.AddItem(circle2);
        m_Primitives.AddItem(circle3);
        m_Primitives.AddItem(circle4);
        m_Primitives.AddItem(circle5);
        auto quad = std::make_shared<SimplePrimitive>(
            std::make_shared<Quad>(10.0f, 10.0f),
            std::make_unique<LambertianMaterial>(glm::vec3{ 0.8f, 0.8f, 0.8f})
        );
        quad->SetTransform(
            glm::vec3{ 1.0f },
            glm::vec3{ 20.0f, 0.0f, 0.0f },
            glm::vec3{ -1.0f, -1.0f, -1.0f }
        );
        m_Primitives.AddItem(quad);
        auto tri = std::make_shared<SimplePrimitive>(
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
            std::make_unique<LambertianMaterial>(glm::vec3{ 0.8f, 0.8f, 0.8f})
        );
        tri->SetTransform(
            glm::vec3{ 1.0f },
            glm::vec3{ -20.0f, 10.0f, 0.0f },
            glm::vec3{ 2.0f, 1.0f, 0.0f }
        );
        // m_Primitives.AddItem(tri);

        auto ply = std::make_shared<Mesh>(
            "../../assets/icosahedron.ply"
        );

        auto triMesh = std::make_shared<TriangleList>(
            *ply,
            std::make_unique<DielectricMaterial>(0.9f)
            // std::make_unique<EmissiveMaterial>(glm::vec3{ 5.0f, 5.0f, 4.0f })
            // std::make_shared<LambertianMaterial>(glm::vec3{ 0.8f, 0.8f, 0.85f})
        );
        triMesh->SetTransform(
            glm::vec3{ 5.0f },
            glm::vec3{ 10.0f, 20.0f, 30.0f },
            glm::vec3{ 0.0f, 10.0f, 0.0f }
        );
        m_Primitives.AddItem(triMesh);
    }

    void Intersect(const Ray& ray, SurfaceInteraction* intersect)
    {
        m_Primitives.Intersect(ray, intersect);
    }

private:
    PrimitiveList m_Primitives;
};