#pragma once

#include "Geometry.h"
#include <memory>


class Material;

struct SurfaceInteraction
{
    glm::vec3 Position{ 0.0f };
    glm::vec3 Normal{ 0.0f, 0.0f, 0.0f };
    bool HasIntersection = false;
    bool IsFrontFace = false;
    Material* Material = nullptr;
};

class Shape
{
public:
    Shape() {};
    virtual void Intersect(const Ray& ray, SurfaceInteraction* intersect) const = 0;
    // TODO: Add Transform parameter
    virtual AABB GetAABB(Transform* transform) const = 0;
};

class Circle : public Shape
{
public:
    Circle(float radius=1.0f) : m_Radius(radius), Shape() {};

    virtual void Intersect(const Ray& ray, SurfaceInteraction* intersect) const override;
    virtual AABB GetAABB(Transform* transform) const override;
    
    private:
    float m_Radius{ 1.0f };
};

class Quad : public Shape
{
    public:
    Quad(float width=1.0f, float height=1.0f) : m_Width(width), m_Height(height) {}
    
    virtual void Intersect(const Ray& ray, SurfaceInteraction* intersect) const override;
    virtual AABB GetAABB(Transform* transform) const override;

private:
    float m_Width;
    float m_Height;

    const glm::vec3 m_Normal = glm::vec3(0.0f, 1.0f, 0.0f);
};

class Triangle : public Shape
{
public:
    Triangle(
        const glm::vec3& v0, 
        const glm::vec3& v1, 
        const glm::vec3& v2,
        const glm::vec3& n0, 
        const glm::vec3& n1, 
        const glm::vec3& n2,
        const glm::vec2& uv0, 
        const glm::vec2& uv1, 
        const glm::vec2& uv2
        )
    {
        m_Vertices[0] = v0;
        m_Vertices[1] = v1;
        m_Vertices[2] = v2;
        
        m_Normals[0] = n0;
        m_Normals[1] = n1;
        m_Normals[2] = n2;

        m_UVs[0] = uv0;
        m_UVs[1] = uv1;
        m_UVs[2] = uv2;
    }

    virtual void Intersect(const Ray& ray, SurfaceInteraction* intersect) const override;
    virtual AABB GetAABB(Transform* transform) const override;

private:
    glm::vec3 m_Vertices[3];
    glm::vec3 m_Normals[3];
    glm::vec2 m_UVs[3];
};