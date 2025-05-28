#include "Shape.h"

SurfaceInteraction Circle::Intersect(const Ray &ray) const
{
    SurfaceInteraction intersect;

    auto rayLocal = ray.Transform(m_Transform.GetInvMat());

    auto l = rayLocal.Origin;
    float a = glm::dot(rayLocal.Direction, rayLocal.Direction);
    float b = 2.0f * glm::dot(l, rayLocal.Direction);
    float c = glm::dot(l,l) - m_Radius * m_Radius;

    float discriminant = b * b - 4.0f * a * c;

    if (discriminant >= 0.0f)
    {
        float t1 = (-b + sqrtf(discriminant)) / 2 * a;
        float t2 = (-b - sqrtf(discriminant)) / 2 * a;
        float t = 0.0f;

        intersect.HasIntersection = true;
        if (t1 >= 0.001f && t2 >= 0.001f)
        {
            t = t1 < t2 ? t1 : t2;
        }
        else if (t1 >= 0.001f)
        {
            t = t1;
        }
        else if (t2 >= 0.001f)
        {
            t = t2;
        }
        else
        {
            intersect.HasIntersection = false;
        }
        auto intersectPoint = rayLocal.Origin + rayLocal.Direction * t;
        auto normal = glm::normalize(intersectPoint);
        intersect.Position = Rotate(m_Transform.GetMat(), intersectPoint);
        intersect.Position = Translate(m_Transform.GetMat(), intersect.Position);
        intersect.Normal = Rotate(m_Transform.GetMat(), normal);
    }
    else
    {
        intersect.HasIntersection = false;
    }

    return intersect;
}

SurfaceInteraction Quad::Intersect(const Ray& ray) const
{
    SurfaceInteraction intersect;

    auto rayLocal = ray.Transform(m_Transform.GetInvMat());

    if (fabs(rayLocal.Direction.y) < 1e-8f)
    {
        intersect.HasIntersection = false;
        return intersect;
    }

    auto t = -rayLocal.Origin.y / rayLocal.Direction.y;

    auto p = rayLocal.Origin + rayLocal.Direction * t;

    float halfWidth = m_Width / 2.0f;
    float halfHeight = m_Height / 2.0f;

    if (t > 0.001f && (p.x * p.x < halfWidth * halfWidth) && (p.z * p.z < halfHeight * halfHeight))
    {
        intersect.HasIntersection = true;
        intersect.Position = Rotate(m_Transform.GetMat(), p);
        intersect.Position = Translate(m_Transform.GetMat(), intersect.Position);
        intersect.Normal = Rotate(m_Transform.GetMat(), rayLocal.Origin.y > 0.0f ? m_Normal : -m_Normal);
    }
    else
    {
        intersect.HasIntersection = false;
    }

    return intersect;
}

SurfaceInteraction Triangle::Intersect(const Ray& ray) const
{
    SurfaceInteraction intersect;
    
    auto rayLocal = ray.Transform(m_Transform.GetInvMat());

    const auto& P0 = m_Vertices[0];
    const auto& P1 = m_Vertices[1];
    const auto& P2 = m_Vertices[2];
    const auto& N0 = m_Normals[0];
    const auto& N1 = m_Normals[1];
    const auto& N2 = m_Normals[2];
    auto S = rayLocal.Origin - P0;
    auto E1 = P1 - P0;
    auto E2 = P2 - P0;
    auto S1 = glm::cross(rayLocal.Direction, E2);
    auto S2 = glm::cross(S, E1);

    auto divisor = glm::dot(S1, E1);
    if (divisor == 0)
    {
        return intersect;
    }

    auto t = glm::dot(S2, E2) / divisor;
    auto b1 = glm::dot(S1, S) / divisor;
    auto b2 = glm::dot(S2, rayLocal.Direction) / divisor;

    if (t < 0.001f || b1 < 0.0f || b2 < 0.0f || b1 + b2 > 1.0f)
    {
        return intersect;
    }

    intersect.Position = (1 - b1 - b2) * P0 + b1 * P1 + b2 * P2;
    intersect.Position = Rotate(m_Transform.GetMat(), intersect.Position);
    intersect.Position = Translate(m_Transform.GetMat(), intersect.Position);
    intersect.Normal = (1 - b1 - b2) * N0 + b1 * N1 + b2 * N2;
    intersect.Normal = Rotate(m_Transform.GetMat(), intersect.Normal);
    intersect.Normal = Translate(m_Transform.GetMat(), rayLocal.Origin.y > 0.0f ? intersect.Normal : -intersect.Normal);
    intersect.HasIntersection = true;


   return intersect;
}