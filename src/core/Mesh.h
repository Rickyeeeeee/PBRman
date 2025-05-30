#pragma once

#include "Core.h"
#include <vector>
#include <string>
#include <iostream>

class Mesh
{
public:
    Mesh(const std::string& plyFilePath);
    const std::vector<glm::vec3>&   GetVertices()   const { return m_Vertices;          }
    const std::vector<glm::vec3>&   GetNormals()    const { return m_Normals;           }
    const std::vector<uint32_t>&    GetIndices()    const { return m_TriangleIndices;   }
private:
    const std::string m_PlyFilePath;
    std::vector<glm::vec3> m_Vertices;
    std::vector<glm::vec3> m_Normals;
    std::vector<glm::vec2> m_TexCoords;
    std::vector<uint32_t> m_TriangleIndices;
};
