#pragma once

#include "Primitive.h"
#include <functional>
#include <memory>

struct BVHPrimitiveInfo
{
    BVHPrimitiveInfo() {}
    BVHPrimitiveInfo(size_t primitiveNumber, const AABB &bounds)
        : PrimitiveIndex(primitiveNumber),
          Bounds(bounds),
          Centroid(.5f * bounds.Min + .5f * bounds.Max) {}
    size_t PrimitiveIndex;
    AABB Bounds;
    glm::vec3 Centroid;
};

struct BVHBuildNode
{
    void InitLeaf(int first, int n, const AABB &b) {
        FirstPrimOffset = first;
        nPrimitives = n;
        Bounds = b;
        Children[0] = Children[1] = nullptr;
    }
    void InitInterior(int axis, BVHBuildNode *c0, BVHBuildNode *c1) {
        Children[0] = c0;
        Children[1] = c1;
        Bounds = AABB::Union(c0->Bounds, c1->Bounds);
        SplitAxis = axis;
        nPrimitives = 0;
    }
    AABB Bounds;
    BVHBuildNode* Children[2];
    int SplitAxis, FirstPrimOffset, nPrimitives;
};


class BVHBuildNodePool
{
public:
    BVHBuildNodePool(size_t capacity)
        : m_Capacity(capacity)
    {
        m_Nodes = new BVHBuildNode[capacity];
    }

    BVHBuildNode* GetNode()
    {
        if (m_Size == m_Capacity)
            std::cout << "BVH build node pools full!" << std::endl;
        return &m_Nodes[m_Size++];
    }

    ~BVHBuildNodePool()
    {
        delete[] m_Nodes;
    }
private:
    BVHBuildNode* m_Nodes = nullptr;
    size_t m_Capacity;
    size_t m_Size = 0;
};

struct LinearBVHNode 
{
    AABB Bounds;
    union 
    {
        int PrimitivesOffset;   // leaf
        int SecondChildOffset;  // interior
    };
    uint16_t nPrimitives;  // 0 -> interior node
    uint8_t axis;
    uint8_t pad[1];
};

class BVH : Primitive
{
public:
    BVH(std::vector<std::shared_ptr<SimplePrimitive>> primitives);

    virtual void Intersect(const Ray& ray, SurfaceInteraction* intersect) override;

    void Traverse(std::function<void(int /* depth */, const AABB& aabb)>);

private:

    BVHBuildNode* RecursiveBuild(
        BVHBuildNodePool& nodePool,
        std::vector<BVHPrimitiveInfo>& primitiveInfo, 
        int start, 
        int end, 
        int *totalNodes,
        std::vector<std::shared_ptr<SimplePrimitive>>& orderedPrims
    );

    int FlattenBVHTree(BVHBuildNode* node, int* offset);

    std::vector<LinearBVHNode> m_Nodes;
    std::vector<std::shared_ptr<SimplePrimitive>> m_Primitives;
    
};