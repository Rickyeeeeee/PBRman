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
        intersect.Normal = Rotate(m_Transform.GetMat(), m_Normal);
    }
    else
    {
        intersect.HasIntersection = false;
    }

    return intersect;
}