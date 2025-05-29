#pragma once

#include "Shape.h"
#include "Material.h"
#include <initializer_list>

class Primitive
{
public:
    Primitive() = default;
    virtual void Intersect(const Ray& ray, SurfaceInteraction* intersect) = 0;
};

class ShapePrimitive : public Primitive
{
public:
    ShapePrimitive(std::shared_ptr<Shape> shape, std::shared_ptr<Material> material)
        : m_Shape(shape), m_Material(material) {}
    virtual void Intersect(const Ray& ray, SurfaceInteraction* intersect) override;
    Shape& GetShape() { return *m_Shape; }
    Material& GetMaterial() { return *m_Material; }
private:
    std::shared_ptr<Shape> m_Shape;
    std::shared_ptr<Material> m_Material;
};

class ShapePrimitiveList : public Primitive
{
public:
    ShapePrimitiveList() = default;

    void AddItem(const ShapePrimitive& shapePrimitive)
    {
        m_List.push_back(shapePrimitive);
    }

    virtual void Intersect(const Ray& ray, SurfaceInteraction* intersect) override;
private:
    std::vector<ShapePrimitive> m_List;
};