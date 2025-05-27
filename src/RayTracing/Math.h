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