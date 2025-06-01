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

    m_Primitives = orderedPrims;

    m_Nodes.resize(totalNodes);

    int offset = 0;
    FlattenBVHTree(root, &offset);
}

void BVH::Intersect(const Ray& ray, SurfaceInteraction* intersect)
{
    if (m_Nodes.empty()) 
    {
        return;
    }

    glm::vec3 invDir(1 / ray.Direction.x, 1 / ray.Direction.y, 1 / ray.Direction.z);
    int dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };

    // Follow ray through BVH nodes to find primitive intersections
    int toVisitOffset = 0, currentNodeIndex = 0;
    int nodesToVisit[64];

    float minDistance = std::numeric_limits<float>::max();
    int minIndex = -1;
    SurfaceInteraction minSurfaceInteraction;

    while (true) {
        const LinearBVHNode *node = &m_Nodes[currentNodeIndex];
        // Check ray against BVH node
        if (node->Bounds.IntersectP(ray)) {
            if (node->nPrimitives > 0) {
                // Intersect ray with primitives in leaf BVH node
                for (int i = 0; i < node->nPrimitives; ++i)
                {
                    SurfaceInteraction si;
                    m_Primitives[node->PrimitivesOffset + i]->Intersect(ray, &si);
                    auto disVec = ray.Origin - si.Position;
                    auto distance2 = glm::dot(disVec, disVec);
                    if (si.HasIntersection && distance2 < minDistance)
                    {
                        minDistance = distance2;
                        minIndex = 0;
                        minSurfaceInteraction = si;
                    }
                }
                if (toVisitOffset == 0) break;
                currentNodeIndex = nodesToVisit[--toVisitOffset];
            } else {
                // Put far BVH node on _nodesToVisit_ stack, advance to near
                // node
                if (dirIsNeg[node->axis]) {
                    nodesToVisit[toVisitOffset++] = node->SecondChildOffset;
                    currentNodeIndex = currentNodeIndex + 1;
                } else {
                    nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
                    currentNodeIndex = node->SecondChildOffset;
                }
            }
        } else {
            if (toVisitOffset == 0) break;
            currentNodeIndex = nodesToVisit[--toVisitOffset];
        }
    }

    if (minIndex >= 0)
    {
        *intersect = minSurfaceInteraction;
        return;
    }

    intersect->HasIntersection = false;
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
            int method = 1;
            if (method == 0)
            {
                // Partition primitives into equally-sized subsets
                mid = (start + end) / 2;
                std::nth_element(&primitiveInfo[start], &primitiveInfo[mid],
                    &primitiveInfo[end - 1] + 1,
                    [dim](const BVHPrimitiveInfo &a,
                        const BVHPrimitiveInfo &b) {
                        return a.Centroid[dim] < b.Centroid[dim];
                    });
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
            }
            
            node->InitInterior(dim, 
                RecursiveBuild(nodePool, primitiveInfo, start, mid, totalNodes, orderedPrims),
                RecursiveBuild(nodePool, primitiveInfo, mid, end, totalNodes, orderedPrims)
            );
            
        }
        
    }
    
    return node;
}

int BVH::FlattenBVHTree(BVHBuildNode* node, int* offset)
{
    LinearBVHNode *linearNode = &m_Nodes[*offset];
    linearNode->Bounds = node->Bounds;
    int myOffset = (*offset)++;
    if (node->nPrimitives > 0) {
        linearNode->PrimitivesOffset = node->FirstPrimOffset;
        linearNode->nPrimitives = node->nPrimitives;
    } else {
        // Create interior flattened BVH node
        linearNode->axis = node->SplitAxis;
        linearNode->nPrimitives = 0;
        FlattenBVHTree(node->Children[0], offset);
        linearNode->SecondChildOffset =
            FlattenBVHTree(node->Children[1], offset);
    }
    return myOffset;
}