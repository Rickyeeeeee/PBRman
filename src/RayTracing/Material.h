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
class MetalMeterial : public Material
{
public:
    MetalMeterial(const glm::vec3& albedo, float metallic) : m_Albedo(albedo), m_Roughness(metallic) {}

    virtual bool Scatter(const Ray& inRay, const SurfaceInteraction& interaction, glm::vec3& attenuation, Ray& outRay) const override
    {
        auto reflectedDir = glm::reflect(inRay.Direction, interaction.Normal);
        reflectedDir = glm::normalize(reflectedDir) + m_Roughness * RandomUnitVector();

        outRay.Origin = interaction.Position;
        outRay.Direction = glm::normalize(reflectedDir);

        attenuation = m_Albedo;

        return glm::dot(outRay.Direction, interaction.Normal) > 0.0f;
    }

private:
    glm::vec3 m_Albedo;
    float m_Roughness; // Fresnel reflectance
};

class dielectricMaterial : public Material
{
public:
    dielectricMaterial(float refractionIndex) : m_RefractionIndex(refractionIndex) {};

    virtual bool Scatter(const Ray& inRay, const SurfaceInteraction& interaction, glm::vec3& attenuation, Ray& outRay) const override
    {
        attenuation = glm::vec3 { 1.0f, 1.0f, 1.0f };
        // double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        // vec3 unit_direction = unit_vector(r_in.direction());
        // double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        // double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

        // bool cannot_refract = ri * sin_theta > 1.0;
        // vec3 direction;

        // if (cannot_refract || reflectance(cos_theta, ri) > random_double())
        //     direction = reflect(unit_direction, rec.normal);
        // else
        //     direction = refract(unit_direction, rec.normal, ri);

        // scattered = ray(rec.p, direction, r_in.time());

        return true;
    }
private:
    float m_RefractionIndex;

    static double fresnelReflectance(float cosine, float refractionIndex) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refractionIndex) / (1 + refractionIndex);
        r0 = r0*r0;
        return r0 + (1-r0)*std::pow((1 - cosine),5);
    }
};