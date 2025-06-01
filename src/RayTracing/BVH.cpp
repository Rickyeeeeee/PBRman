#include "BVH.h"

#include <algorithm>

BVH::BVH(std::vector<std::shared_ptr<SimplePrimitive>> primitives)
    : m_Primitives(std::move(primitives))
{
    if (m_Primitives.empty()) return;

    std::vector<BVHPrimitiveInfo> primitiveInfos(m_Primitives.size());

    for (size_t i = 0; i < m_Primitives.size(); i++)
    {
        primitiveInfos[i] = {
            i, m_Primitives[i]->GetAABB()
        };
    }

    std::vector<std::shared_ptr<SimplePrimitive>> orderedPrims;
    orderedPrims.reserve(m_Primitives.size());

    BVHBuildNodePool nodePool(m_Primitives.size() * 2 + 2);
    
    int totalNodes = 0;
    BVHBuildNode *root;

    root = RecursiveBuild(nodePool, primitiveInfos, 0, m_Primitives.size(), &totalNodes, orderedPrims);

    m_Nodes.resize(totalNodes);

    int offset = 0;
    FlattenBVHTree(root, &offset);
}

void BVH::Intersect(const Ray& ray, SurfaceInteraction* intersect)
{
    
}

void BVH::Traverse(std::function<void(int, const AABB&)> callback)
{
    int depth = 0;

    int nodesStack[64];
    int nodesTop = 0;
    nodesStack[nodesTop] = 0;

    while (true)
    {
        if (nodesTop < 0)
            break;

        const auto& curBVHNode = m_Nodes[nodesStack[nodesTop]];

        callback(curBVHNode.nPrimitives, curBVHNode.Bounds);

        nodesTop--;

        if (curBVHNode.nPrimitives == 0)
        {
            nodesTop++;
            nodesStack[nodesTop] = nodesStack[nodesTop] + 1;
            nodesTop++;
            nodesStack[nodesTop] = curBVHNode.SecondChildOffset;
        }
    }
}

BVHBuildNode* BVH::RecursiveBuild(
    BVHBuildNodePool& nodePool,
    std::vector<BVHPrimitiveInfo>& primitiveInfo, 
    int start, 
    int end, 
    int *totalNodes,
    std::vector<std::shared_ptr<SimplePrimitive>>& orderedPrims
)
{
    BVHBuildNode* node = nodePool.GetNode();

    (*totalNodes)++;

    AABB bounds;
    for (int i = start; i < end; i++)
        bounds = AABB::Union(bounds, primitiveInfo[i].Bounds);

    int nPrimitives = end - start;
    if (nPrimitives == 1)
    {
        int firstPrimOffset = orderedPrims.size();
        for (int i = start; i < end; i++)
        {
            int primNum = primitiveInfo[i].PrimitiveIndex;
            orderedPrims.push_back(m_Primitives[primNum]);
        }
        node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
        return node;
    }
    else
    {
        AABB centroidBounds;
        for (int i = start; i < end; i++)
        {
            centroidBounds = AABB::Union(centroidBounds, primitiveInfo[i].Centroid);
        }
        int dim = centroidBounds.MaxExtent();

        int mid = (start + end) / 2;
        if (centroidBounds.Max[dim] == centroidBounds.Min[dim])
        {
            int firstPrimOffset = orderedPrims.size();
            for (int i = start; i < end; ++i) {
                int primNum = primitiveInfo[i].PrimitiveIndex;
                orderedPrims.push_back(m_Primitives[primNum]);
            }
            node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
            return node;
        }
        else
        {
            // Partition primitives into equally-sized subsets
            mid = (start + end) / 2;
            std::nth_element(&primitiveInfo[start], &primitiveInfo[mid],
                &primitiveInfo[end - 1] + 1,
                [dim](const BVHPrimitiveInfo &a,
                    const BVHPrimitiveInfo &b) {
                    return a.Centroid[dim] < b.Centroid[dim];
                });

            node->InitInterior(dim, 
                RecursiveBuild(nodePool, primitiveInfo, start, mid, totalNodes, orderedPrims),
                RecursiveBuild(nodePool, primitiveInfo, mid, end, totalNodes, orderedPrims)
            );
        }
        
    }
    
    return node;
}