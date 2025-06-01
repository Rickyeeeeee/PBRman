#pragma once

#include "core/Core.h"

struct Ray
{
    glm::vec3 Origin{ 0.0f };
    glm::vec3 Direction{ 0.0f, 0.0f, 1.0f };

    void Normalize()
    {
        Direction = glm::normalize(Direction);
    }

    const Ray& ApplyRotate(const glm::vec3& xAxis, const glm::vec3& yAxis, const glm::vec3& zAxis)
    {
        Direction = Direction.x * xAxis + Direction.y * yAxis + Direction.z * zAxis;
        
        return *this;
    }
    
    const Ray& ApplyOffset(const glm::vec3& offset)
    {
        Origin += offset;
        
        return *this;
    }
    
    Ray Rotate(const glm::vec3& xAxis, const glm::vec3& yAxis, const glm::vec3& zAxis) const
    {
        Ray ray = *this;
        ray.Direction = Direction.x * xAxis + Direction.y * yAxis + Direction.z * zAxis;

        return ray;
    }

    Ray Offset(const glm::vec3& offset) const
    {
        Ray ray = *this;
        ray.Origin += offset;

        return ray;
    }
    
    const Ray& ApplyTransform(const glm::mat4& mat)
    {
        Origin = glm::vec3(mat * glm::vec4(Origin, 1.0f));
        Direction = mat * glm::vec4(Direction, 0.0f);
    } 
    
    Ray Transform(const glm::mat4& mat) const
    {
        Ray ray = *this;
        ray.Origin = glm::vec3(mat * glm::vec4(ray.Origin, 1.0f));
        ray.Direction = mat * glm::vec4(Direction, 0.0f);

        return ray;
    }
};

class Transform
{
public:
    Transform() = default;
    
    Transform(const glm::mat4& mat)
    {
        m_Mat = mat;
        this->UpdateInv();
    }

    Transform(const glm::mat4& mat, const glm::mat4& invMat)
        : m_Mat(mat), m_InvMat(invMat) {}

    void SetMat(const glm::mat4& mat)
    {
        m_Mat = mat;
        this->UpdateInv();
    }

    void Set(const glm::vec3& scale, const glm::vec3& eulerAngles, const glm::vec3& translation)
    {
        m_Mat = 
            glm::translate(glm::mat4(1.0f), translation) * 
            glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z) * 
            glm::scale(glm::mat4(1.0f), scale);
        this->UpdateInv();
    }
    
    Transform GetInverse() const
    {
        return Transform{ m_InvMat, m_Mat };
    }

    const glm::mat4& GetMat() const
    {
        return m_Mat;
    }
    
    const glm::mat4& GetInvMat() const
    {
        return m_InvMat;
    }
    
private:
    
    void UpdateInv()
    {
        m_InvMat = glm::inverse(m_Mat);
    }
    
private:
    glm::mat4 m_Mat{ 1.0f };
    glm::mat4 m_InvMat{ 1.0f };
};

inline glm::vec3 TransformVector(const glm::mat4& mat, const glm::vec3 &vec)
{
    return mat * glm::vec4(vec, 0.0f);
}

inline glm::vec3 TransformNormal(const glm::mat4& invMat, const glm::vec3& normal)
{
    return glm::normalize(glm::mat3(glm::transpose(invMat)) * normal);
}

inline glm::vec3 TransformPoint(const glm::mat4& mat, const glm::vec3& point)
{
    return mat * glm::vec4(point, 1.0f);
}

struct AABB
{
    glm::vec3 Min{ std::numeric_limits<float>::max() };
    glm::vec3 Max{ std::numeric_limits<float>::lowest() };

    static AABB Union(const AABB& box1, const AABB& box2)
    {
        return AABB{
            glm::min(box1.Min, box2.Min),
            glm::max(box1.Max, box2.Max)
        };
    }

    static AABB Union(const AABB& bound, const glm::vec3& point)
    {
        return AABB{
            glm::min(bound.Min, point),
            glm::max(bound.Max, point)
        };
    }

    int MaxExtent() const
    {
        auto extent = Max - Min;

        if (extent.x >= extent.y && extent.x >= extent.z)
            return 0;
        else if (extent.y >= extent.x && extent.y >= extent.z)
            return 1;
        else
            return 2;
    }

    AABB TransformAndBound(Transform* transform)
    {
        glm::vec3 v[8] = {
            { Max.x, Max.y, Max.z },
            { Max.x, Max.y, Min.z },
            { Max.x, Min.y, Max.z },
            { Max.x, Min.y, Min.z },
            { Min.x, Max.y, Max.z },
            { Min.x, Max.y, Min.z },
            { Min.x, Min.y, Max.z },
            { Min.x, Min.y, Min.z },
        };

        glm::vec3 newMin { TransformPoint(transform->GetMat(), v[0]) };
        glm::vec3 newMax { TransformPoint(transform->GetMat(), v[0]) };

        for (int i = 1; i < 8; i++)
        {
            auto p = TransformPoint(transform->GetMat(), v[i]);

            newMax = glm::max(newMax, p);
            newMin = glm::min(newMin, p);
        }

        return AABB{
            newMin,
            newMax
        };
    }
};
