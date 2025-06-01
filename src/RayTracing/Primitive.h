#pragma once

#include "Shape.h"
#include "Material.h"
#include <initializer_list>
#include "core/Mesh.h"

class Primitive
{
public:
    Primitive() = default;
    virtual void Intersect(const Ray& ray, SurfaceInteraction* intersect) = 0;
};

class SimplePrimitive : public Primitive
{
public:
    SimplePrimitive(std::shared_ptr<Shape> shape, std::shared_ptr<Material> material, Transform transform=Transform())
        : m_Shape(shape), m_Material(material), m_Transform(transform) {}
    virtual void Intersect(const Ray& ray, SurfaceInteraction* intersect) override;
    Shape& GetShape() { return *m_Shape; }
    Material& GetMaterial()                     { return *m_Material; }

    Transform GetTransform() const              { return m_Transform; }
    void SetTransform(const Transform& trans)   { m_Transform = trans; }
    void SetTransform(
        const glm::vec3& scale, 
        const glm::vec3& eulerAngles, 
        const glm::vec3& translation)   
    { 
        m_Transform = Transform();
        m_Transform.Set(scale, glm::radians(eulerAngles), translation);
    }

    AABB GetAABB()
    {
        return m_Shape->GetAABB(&m_Transform);
    }
private:
    std::shared_ptr<Shape> m_Shape;
    Transform m_Transform;
    std::shared_ptr<Material> m_Material;
};

class PrimitiveList : public Primitive
{
public:
    PrimitiveList() = default;

    void AddItem(std::shared_ptr<Primitive> shapePrimitive)
    {
        m_List.push_back(shapePrimitive);
    }

    virtual void Intersect(const Ray& ray, SurfaceInteraction* intersect) override;
private:
    std::vector<std::shared_ptr<Primitive>> m_List;
};

class TriangleList : public Primitive
{
public:
    TriangleList(const Mesh& mesh, std::shared_ptr<Material> material);
    virtual void Intersect(const Ray& ray, SurfaceInteraction* intersect) override;
    Transform GetTransform() const              { return m_Transform; }
    std::vector<std::shared_ptr<SimplePrimitive>> GetPrimitives() const;
    void SetTransform(const Transform& trans)   { m_Transform = trans; }
    void SetTransform(
        const glm::vec3& scale, 
        const glm::vec3& eulerAngles, 
        const glm::vec3& translation)   
    { 
        m_Transform = Transform(); 
        m_Transform.Set(scale, glm::radians(eulerAngles), translation);
    }
private:
    std::vector<Triangle>       m_TriangleList;
    std::shared_ptr<Material>   m_Material;
    Transform                   m_Transform;
};