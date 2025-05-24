#pragma once

#include "Geometry.h"

struct Intersection
{
    bool HasIntersection = false;
    glm::vec3 Position{ 0.0f };
    glm::vec3 Normal{ 0.0f, 0.0f, 0.0f };
};

class Shape
{
public:
    Shape() {};
    virtual Intersection Intersect(const Ray& ray) = 0;
    Transform GetTransform() const { return m_Transform; }
    void SetTransform(const Transform& trans) {  m_Transform = trans; }

protected:
    Transform m_Transform;
};

class Circle : public Shape
{
public:
    Circle() : Shape() {};

    virtual Intersection Intersect(const Ray& ray) override;
    
private:
    float m_Radius{ 1.0f };
};