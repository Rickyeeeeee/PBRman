#include "Shape.h"

constexpr static float tMin = 0.001f;

void Circle::Intersect(const Ray &ray, SurfaceInteraction* intersect) const
{
    auto l = ray.Origin;
    float a = glm::dot(ray.Direction, ray.Direction);
    float b = 2.0f * glm::dot(l, ray.Direction);
    float c = glm::dot(l,l) - m_Radius * m_Radius;

    float discriminant = b * b - 4.0f * a * c;

    if (discriminant >= 0.0f)
    {
        float t1 = (-b + sqrtf(discriminant)) / 2 * a;
        float t2 = (-b - sqrtf(discriminant)) / 2 * a;
        float t = 0.0f;

        intersect->HasIntersection = true;
        if (t1 >= tMin && t2 >= tMin)
        {
            t = t1 < t2 ? t1 : t2;
            intersect->IsFrontFace = true;
        }
        else if (t1 >= tMin)
        {
            t = t1;
            intersect->IsFrontFace = false;
        }
        else if (t2 >= tMin)
        {
            t = t2;
            intersect->IsFrontFace = false;
        }
        else
        {
            intersect->HasIntersection = false;
        }
        auto intersectPoint = ray.Origin + ray.Direction * t;
        auto normal = glm::normalize(intersectPoint);
        if (!intersect->IsFrontFace)
            normal *= -1.0f;
        intersect->Position = intersectPoint;
        intersect->Normal = normal;
    }
    else
    {
        intersect->HasIntersection = false;
    }

}

AABB Circle::GetAABB() const
{
    return AABB{
        glm::vec3{ -m_Radius },
        glm::vec3{  m_Radius }
    };
}

void Quad::Intersect(const Ray& ray, SurfaceInteraction* intersect) const
{
    if (fabs(ray.Direction.y) < 1e-8f)
    {
        intersect->HasIntersection = false;
        return;
    }

    auto t = -ray.Origin.y / ray.Direction.y;

    auto p = ray.Origin + ray.Direction * t;

    float halfWidth = m_Width / 2.0f;
    float halfHeight = m_Height / 2.0f;

    if (t > tMin && (p.x * p.x < halfWidth * halfWidth) && (p.z * p.z < halfHeight * halfHeight))
    {
        intersect->HasIntersection = true;
        intersect->Position = p;
        intersect->IsFrontFace = ray.Origin.y > 0.0f;
        intersect->Normal = intersect->IsFrontFace ? m_Normal : -m_Normal;
    }
    else
    {
        intersect->HasIntersection = false;
    }
}

AABB Quad::GetAABB() const
{
    return AABB{
		glm::vec3{ -m_Width / 2.0f, -tMin, -m_Height / 2.0f },
		glm::vec3{  m_Width / 2.0f,  tMin,  m_Height / 2.0f }
    };
}

void Triangle::Intersect(const Ray& ray, SurfaceInteraction* intersect) const
{
    const auto& P0 = m_Vertices[0];
    const auto& P1 = m_Vertices[1];
    const auto& P2 = m_Vertices[2];
    const auto& N0 = m_Normals[0];
    const auto& N1 = m_Normals[1];
    const auto& N2 = m_Normals[2];
    auto S = ray.Origin - P0;
    auto E1 = P1 - P0;
    auto E2 = P2 - P0;
    auto S1 = glm::cross(ray.Direction, E2);
    auto S2 = glm::cross(S, E1);

    auto divisor = glm::dot(S1, E1);
    if (divisor == 0)
    {
        return;
    }

    auto t = glm::dot(S2, E2) / divisor;
    auto b1 = glm::dot(S1, S) / divisor;
    auto b2 = glm::dot(S2, ray.Direction) / divisor;

    if (t < tMin || b1 < 0.0f || b2 < 0.0f || b1 + b2 > 1.0f)
    {
        return;
    }

    intersect->Position = (1 - b1 - b2) * P0 + b1 * P1 + b2 * P2;
    intersect->Normal = (1 - b1 - b2) * N0 + b1 * N1 + b2 * N2;
    intersect->HasIntersection = true;
    if (glm::dot(intersect->Normal, ray.Direction) > 0.0f)
    {
        intersect->Normal *= -1.0f;
        intersect->IsFrontFace = false;
    }
    else
    {
        intersect->IsFrontFace = true;
    }
}

AABB Triangle::GetAABB() const
{
    return AABB
    {
		glm::min(m_Vertices[0], glm::min(m_Vertices[1], m_Vertices[2])),
		glm::max(m_Vertices[0], glm::max(m_Vertices[1], m_Vertices[2]))
    };
}
