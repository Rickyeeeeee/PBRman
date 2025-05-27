#pragma once

#include "Geometry.h"
#include <memory>


class Material;

struct SurfaceInteraction
{
    bool HasIntersection = false;
    glm::vec3 Position{ 0.0f };
    glm::vec3 Normal{ 0.0f, 0.0f, 0.0f };
    std::shared_ptr<Material> Material;
};

class Shape
{
public:
    Shape() {};
    virtual SurfaceInteraction Intersect(const Ray& ray) const = 0;
    Transform GetTransform() const { return m_Transform; }
    void SetTransform(const Transform& trans) {  m_Transform = trans; }
    void SetRotation(const glm::vec3& eulerAngles) { m_Transform.SetRotation(eulerAngles); }
    void SetTranslation(const glm::vec3& offset) { m_Transform.SetTranslation(offset); }

protected:
    Transform m_Transform;
};

class Circle : public Shape
{
public:
    Circle(float radius=1.0f) : m_Radius(radius), Shape() {};

    virtual SurfaceInteraction Intersect(const Ray& ray) const override;
    
private:
    float m_Radius{ 1.0f };
};

class Quad : public Shape
{
public:
    Quad(float width=1.0f, float height=1.0f) : m_Width(width), m_Height(height) {}

    virtual SurfaceInteraction Intersect(const Ray& ray) const override;

private:
    float m_Width;
    float m_Height;

    const glm::vec3 m_Normal = glm::vec3(0.0f, 1.0f, 0.0f);
};