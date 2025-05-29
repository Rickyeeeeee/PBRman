#include "Primitive.h"

void ShapePrimitive::Intersect(const Ray& ray, SurfaceInteraction* intersect)
{
    m_Shape->Intersect(ray, intersect);
    intersect->Material = m_Material.get();
}

void ShapePrimitiveList::Intersect(const Ray& ray, SurfaceInteraction* intersect)
{
    float minDistance = std::numeric_limits<float>::max();
    int minIndex = -1;
    SurfaceInteraction minSurfaceInteraction;
    for (int i = 0; i < m_List.size(); i++)
    {
        auto& primitive = m_List[i];
        SurfaceInteraction si;
        primitive.Intersect(ray, &si);
        auto disVec = ray.Origin - si.Position;
        auto distance2 = glm::dot(disVec, disVec);
        if (si.HasIntersection && distance2 < minDistance)
        {
            minDistance = distance2;
            minIndex = i;
            minSurfaceInteraction = si;
        }
    }

    if (minIndex >= 0)
    {
        *intersect = minSurfaceInteraction;
        return;
    }

    intersect->HasIntersection = false;
}