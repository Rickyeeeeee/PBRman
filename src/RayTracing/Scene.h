#pragma once

#include "Primitive.h"
#include <vector>
#include "Core/Mesh.h"
#include "BVH.h"

class Scene
{
public:
    Scene() 
    {
        std::vector<std::shared_ptr<SimplePrimitive>> simplePrimitives;

        auto circle = std::make_shared<SimplePrimitive>(
            std::make_shared<Circle>(1.0f),
            // std::make_unique<MetalMaterial>(glm::vec3{ 1.0f, 1.0f, 1.0f })
            // std::make_unique<DielectricMaterial>(0.9f)
            std::make_unique<EmissiveMaterial>(glm::vec3{ 1.0f })
        );
        circle->SetTransform(
            glm::vec3{ 5.0f },
            glm::vec3{ 0.0f },
            glm::vec3{ 0.0f, 10.0f, 0.0f }
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
            // std::make_unique<LambertianMaterial>(glm::vec3{ 1.0f, 1.0f, 1.0f })
            std::make_shared<DielectricMaterial>(0.9f)
        );
        circle4->SetTransform(
            glm::vec3{ 1.0f },
            glm::vec3{ 0.0f },
            glm::vec3{ 4.0f, 1.0f, 0.0f }
        );
        auto circle5 = std::make_shared<SimplePrimitive>(
            std::make_shared<Circle>(1.0f),
            std::make_unique<MetalMaterial>(glm::vec3{ 1.0f, 0.7, 0.8f }, 0.1f)
        );
        circle5->SetTransform(
            glm::vec3{ 1.0f },
            glm::vec3{ 0.0f },
            glm::vec3{ -4.0f, 1.0f, 0.0f }
        );
        m_Primitives.AddItem(circle);
        m_Primitives.AddItem(circle2);
        m_Primitives.AddItem(circle3);
        m_Primitives.AddItem(circle4);
        m_Primitives.AddItem(circle5);
        simplePrimitives.push_back(circle);
        simplePrimitives.push_back(circle2);
        simplePrimitives.push_back(circle3);
        simplePrimitives.push_back(circle4);
        simplePrimitives.push_back(circle5);
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
        simplePrimitives.push_back(quad);
        
        auto ply = std::make_shared<Mesh>(
            // "../../assets/icosahedron.ply"
            // "../../assets/cube_uv.ply"
            "../../assets/bunny.ply"
        );

        auto triMesh = std::make_shared<TriangleList>(
            *ply,
            // std::make_unique<DielectricMaterial>(0.9f)
            // std::make_unique<EmissiveMaterial>(glm::vec3{ 5.0f, 5.0f, 4.0f })
            // std::make_shared<MetalMaterial>(glm::vec3{ 1.0f, 1.0f, 1.0f }, 0.1f)
            std::make_shared<LambertianMaterial>(glm::vec3{ 0.8f, 0.7f, 0.85f})
        );
        triMesh->SetTransform(
            glm::vec3{ 1.0f },
            glm::vec3{ -90.0f, 0.0f, 0.0f },
            glm::vec3{ 0.0f, 1.0f, 0.0f }
        );
        m_Primitives.AddItem(triMesh);
        auto triangles = triMesh->GetPrimitives();
        simplePrimitives.insert(simplePrimitives.begin(), triangles.begin(), triangles.end());
        m_BVH = std::make_shared<BVH>(simplePrimitives);
    }

    void Intersect(const Ray& ray, SurfaceInteraction* intersect)
    {
        // m_Primitives.Intersect(ray, intersect);
        m_BVH->Intersect(ray, intersect);
    }

    // FOr debug purposes
    BVH& GetBVH() 
    {
        return *m_BVH;
    }

private:
    PrimitiveList m_Primitives;
    std::shared_ptr<BVH> m_BVH;
};