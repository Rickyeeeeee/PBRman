#pragma once

#include "Shape.h"
#include "Material.h"

struct Primitive
{
    std::shared_ptr<Shape> Shape;
    std::shared_ptr<Material> Material;
};