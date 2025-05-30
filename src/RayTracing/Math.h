#pragma once

#include "Core/Core.h"
#include <cmath>

inline float Random()
{
    return std::rand() / (RAND_MAX + 1.0f);
}

inline float Random(float min, float max)
{
    return min + (max - min) * Random();
}

inline glm::vec3 RandomUnitVector()
{
    constexpr float epsilon = 1e-8f;
    while (true)
    {
        glm::vec3 p = { Random(-1, 1), Random(-1, 1), Random(-1, 1) };
        auto lensq = glm::dot(p, p);
        if (epsilon < lensq && lensq <= 1.0f)
            return p / sqrt(lensq);
    }
}

inline float LengthSquared(const glm::vec3& v)
{
    return glm::dot(v, v);
}

inline glm::vec3 Reflect(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat) {
    auto cos_theta = std::fmin(dot(-uv, n), 1.0f);
    glm::vec3 r_out_perp =  etai_over_etat * (uv + cos_theta * n);
    glm::vec3 r_out_parallel = -std::sqrt(std::fabs(1.0f - LengthSquared(r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
}