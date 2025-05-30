#pragma once

#include "Geometry.h"
#include "Shape.h"
#include "Math.h"

class Material
{
public:
    Material() = default;
    virtual ~Material() = default;

    virtual bool Scatter(const Ray& inRay, const SurfaceInteraction& intersection, glm::vec3& attenuation, Ray& outRay) const = 0;

    virtual void Emit(const SurfaceInteraction& intersection, glm::vec3& emittedColor) const
    {
        emittedColor = glm::vec3(0.0f); // Default to no emission
    }
};

// outRay = reflect ray + subsurface scattering ray
// reflect ray = 0.0f
// subsurface scattering ray = 1.0f (Randomness because of subsurface scattering)
class LambertianMaterial : public Material
{
public:
    LambertianMaterial(const glm::vec3& albedo) : m_Albedo(albedo) {};

    virtual bool Scatter(const Ray& inRay, const SurfaceInteraction& interaction, glm::vec3& attenuation, Ray& outRay) const override
    {
        auto scatterDirection = interaction.Normal + RandomUnitVector();

        auto& e = scatterDirection;
        auto s = 1e-8;
        if ((std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s))
        {
            scatterDirection = interaction.Normal;
        }

        outRay.Origin = interaction.Position;
        outRay.Direction = glm::normalize(scatterDirection);

        attenuation = m_Albedo;

        return true;
    }

private:

    glm::vec3 m_Albedo;
};

// outRay = reflect ray + subsurface scattering ray
// reflect ray = 1.0f (randomness because of PBR roughness)
// subsurface scattering ray = 0.0f
class MetalMaterial : public Material
{
public:
    MetalMaterial(const glm::vec3& albedo, float metallic=0.0f) : m_Albedo(albedo), m_Roughness(metallic) {}

    virtual bool Scatter(const Ray& inRay, const SurfaceInteraction& interaction, glm::vec3& attenuation, Ray& outRay) const override
    {
        auto reflectedDir = glm::reflect(inRay.Direction, interaction.Normal);
        reflectedDir = glm::normalize(reflectedDir) + m_Roughness * RandomUnitVector();
        // reflectedDir = glm::normalize(reflectedDir);

        outRay.Origin = interaction.Position;
        outRay.Direction = glm::normalize(reflectedDir);

        attenuation = m_Albedo;

        return glm::dot(outRay.Direction, interaction.Normal) > 0.0f;
    }

private:
    glm::vec3 m_Albedo;
    float m_Roughness; // Fresnel reflectance
};

class DielectricMaterial : public Material
{
public:
    DielectricMaterial(float refractionIndex) : m_RefractionIndex(refractionIndex) {};

    virtual bool Scatter(const Ray& inRay, const SurfaceInteraction& interaction, glm::vec3& attenuation, Ray& outRay) const override
    {
        attenuation = glm::vec3 { 1.0f, 1.0f, 1.0f };
        float ri = interaction.IsFrontFace ? (1.0f / m_RefractionIndex) : m_RefractionIndex;

        glm::vec3 unit_direction = inRay.Direction;
        float cos_theta = std::fmin(glm::dot(-unit_direction, interaction.Normal), 1.0f);
        float sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        glm::vec3 direction;

        if (cannot_refract || this->fresnelReflectance(cos_theta, ri) > Random())
            direction = glm::reflect(unit_direction, interaction.Normal);
        else
            direction = Reflect(unit_direction, interaction.Normal, ri);

        outRay = Ray{ interaction.Position, direction };

        return true;
    }
private:
    float m_RefractionIndex;

    static float fresnelReflectance(float cosine, float refractionIndex) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refractionIndex) / (1 + refractionIndex);
        r0 = r0*r0;
        return r0 + (1-r0)*std::pow((1 - cosine),5);
    }
};