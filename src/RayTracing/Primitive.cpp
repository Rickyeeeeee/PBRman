#include "Primitive.h"

void SimplePrimitive::Intersect(const Ray& ray, SurfaceInteraction* intersect)
{
    auto rayLocal = ray.Transform(m_Transform.GetInvMat());
    m_Shape->Intersect(rayLocal, intersect);
    intersect->Position = Rotate(m_Transform.GetMat(), intersect->Position);
    intersect->Position = Translate(m_Transform.GetMat(), intersect->Position);
    intersect->Normal = Rotate(m_Transform.GetMat(), intersect->Normal);
    intersect->Material = m_Material.get();
}

static bool genNormal = true;

TriangleList::TriangleList(const Mesh& mesh, std::shared_ptr<Material> material)
    : m_Material(material)
{
    const auto& vertices = mesh.GetVertices();
    const auto& normals = mesh.GetNormals();
    const auto& indices = mesh.GetIndices();

    if (indices.size() % 3 != 0)
        std::cout << "Indice size is not 3 * n" << std::endl;
    auto count = indices.size() / 3;
    // count = 1000;
    m_TraingleList.reserve(count);

    for (int i = 0; i < count; i++)
    {
        auto i0 = indices[i * 3 + 0];
        auto i1 = indices[i * 3 + 1];
        auto i2 = indices[i * 3 + 2];
        glm::vec3 normal;
        normal = glm::normalize(glm::cross(vertices[i1]-vertices[i0], vertices[i2]-vertices[i0]));
        if (genNormal || normals.size() == 0)
        {
            m_TraingleList.push_back(Triangle(
                vertices[i0] * 1.0f,
                vertices[i1] * 1.0f,
                vertices[i2] * 1.0f,
                normal,
                normal,
                normal,
                glm::vec2{ 0.0f },
                glm::vec2{ 0.0f },
                glm::vec2{ 0.0f } )
            );
        }
        else
        {
            m_TraingleList.push_back(Triangle(
                vertices[i0] * 1.0f,
                vertices[i1] * 1.0f,
                vertices[i2] * 1.0f,
                normals[i0],
                normals[i1],
                normals[i2],
                glm::vec2{ 0.0f },
                glm::vec2{ 0.0f },
                glm::vec2{ 0.0f } )
            );
        }
    }
}

void TriangleList::Intersect(const Ray& ray, SurfaceInteraction* intersect)
{
    auto rayLocal = ray.Transform(m_Transform.GetInvMat());

    float minDistance = std::numeric_limits<float>::max();
    int minIndex = -1;
    SurfaceInteraction minSurfaceInteraction;
	for (uint32_t i = 0; i < m_TraingleList.size(); i++)
	{
        auto& triangle = m_TraingleList[i];
		SurfaceInteraction si;
		triangle.Intersect(rayLocal, &si);
		if (si.HasIntersection)
		{
            si.Position = Rotate(m_Transform.GetMat(), si.Position);
			si.Position = Translate(m_Transform.GetMat(), si.Position);
            
            auto disVec = ray.Origin - si.Position;
            auto distance2 = glm::dot(disVec, disVec);
			si.Normal = Rotate(m_Transform.GetMat(), si.Normal);
			si.Material = m_Material.get();
            if (distance2 < minDistance)
            {
                minDistance = distance2;
                minIndex = i;
                minSurfaceInteraction = si;
            }
		}
	}

    if (minIndex >= 0)
    {
        *intersect = minSurfaceInteraction;
        return;
    }

    intersect->HasIntersection = false; 
}

void PrimitiveList::Intersect(const Ray& ray, SurfaceInteraction* intersect)
{
    float minDistance = std::numeric_limits<float>::max();
    int minIndex = -1;
    SurfaceInteraction minSurfaceInteraction;
    for (int i = 0; i < m_List.size(); i++)
    {
        Primitive* primitive = m_List[i].get();
        SurfaceInteraction si;
        primitive->Intersect(ray, &si);
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