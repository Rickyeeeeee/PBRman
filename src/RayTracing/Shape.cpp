#include "Shape.h"

Intersection Circle::Intersect(const Ray &ray)
{
    Intersection intersect;

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
        if (t1 >= 0.0f && t2 >= 0.0f)
        {
            t = t1 < t2 ? t1 : t2;
        }
        else if (t1 >= 0.0f)
        {
            t = t1;
        }
        else if (t2 >= 0.0f)
        {
            t = t2;
        }
        else
        {
            intersect.HasIntersection = false;
        }
        auto intersectPoint = rayLocal.Origin + rayLocal.Direction * t;
        auto normal = glm::normalize(intersectPoint);
        intersect.Position = Translate(m_Transform.GetMat(), intersectPoint);
        intersect.Normal = Rotate(m_Transform.GetMat(), normal);
    }
    else
    {
        intersect.HasIntersection = false;
    }

    return intersect;
}