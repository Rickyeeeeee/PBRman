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
        Origin += mat[3];
        Direction = mat * glm::vec4(Direction, 0.0f);
    } 
    
    Ray Transform(const glm::mat4& mat) const
    {
        Ray ray = *this;
        ray.Origin += mat[3];
        ray.Direction = mat * glm::vec4(Direction, 0.0f);

        return ray;
    }
};

class Transform
{
public:
    void SetRotation(const glm::vec3& eulerAngles)
    {
        auto rot = glm::mat3(glm::yawPitchRoll(eulerAngles.x, eulerAngles.y, eulerAngles.z));
        SetMat3(rot);
        UpdateInv();
    }

    void SetTranslation(const glm::vec3& trans)
    {
        m_Mat[3] = glm::vec4{ trans, 1.0f};
        UpdateInv();
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
    void SetMat3(const glm::mat3& mat3)
    {
        for (int i = 0; i < 3; i++)
            m_Mat[i] = glm::vec4(mat3[i], m_Mat[i].w);
    }

    void UpdateInv()
    {
        m_InvMat = glm::inverse(m_Mat);
    }

private:
    glm::mat4 m_Mat{ 1.0f };
    glm::mat4 m_InvMat{ 1.0f };
};

inline glm::vec3 Rotate(const glm::mat4& mat, const glm::vec3& vec)
{
    return  mat * glm::vec4(vec, 0.0f);
}

inline glm::vec3 Translate(const glm::mat4& mat, const glm::vec3& point)
{
    return { mat[3][0] + point.x, mat[3][1] + point.y, mat[3][2] + point.z };
}

