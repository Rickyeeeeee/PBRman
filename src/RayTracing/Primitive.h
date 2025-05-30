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
    SimplePrimitive(std::shared_ptr<Shape> shape, std::shared_ptr<Material> material)
        : m_Shape(shape), m_Material(material) {}
    virtual void Intersect(const Ray& ray, SurfaceInteraction* intersect) override;
    Shape& GetShape() { return *m_Shape; }
    Material& GetMaterial() { return *m_Material; }
    Transform GetTransform() const                      { return m_Transform; }
    SimplePrimitive& SetTransform(const Transform& trans)         { m_Transform = trans;                  return *this; }
    SimplePrimitive& SetRotation(const glm::vec3& eulerAngles)    { m_Transform.SetRotation(eulerAngles); return *this; }
    SimplePrimitive& SetTranslation(const glm::vec3& offset)      { m_Transform.SetTranslation(offset);   return *this; }
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
    Transform GetTransform() const                      { return m_Transform; }
    TriangleList& SetTransform(const Transform& trans)         { m_Transform = trans;                  return *this; }
    TriangleList& SetRotation(const glm::vec3& eulerAngles)    { m_Transform.SetRotation(eulerAngles); return *this; }
    TriangleList& SetTranslation(const glm::vec3& offset)      { m_Transform.SetTranslation(offset);   return *this; }
private:
    std::vector<Triangle>       m_TraingleList;
    std::shared_ptr<Material>   m_Material;
    Transform                   m_Transform;
};