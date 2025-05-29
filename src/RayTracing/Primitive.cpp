#include "Primitive.h"

void ShapePrimitive::Intersect(const Ray& ray, SurfaceInteraction* intersect) const
{
    m_Shape->Intersect(ray, intersect);
    intersect->Material = m_Material;
}