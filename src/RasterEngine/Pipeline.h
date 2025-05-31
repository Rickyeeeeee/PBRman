// FullscreenQuadPipeline.h

#pragma once

#include <nvrhi/nvrhi.h>
#include <DirectXMath.h>
#include "core/Core.h"

class FullscreenQuadPipeline
{
public:
    FullscreenQuadPipeline(nvrhi::IDevice* device, nvrhi::IFramebuffer* framebuffer);
    ~FullscreenQuadPipeline();

    void Render(nvrhi::ICommandList* commandList, nvrhi::ITexture* texture, nvrhi::ISampler* sampler, float width, float height);

private:
    void CreateVertexBuffer();
    void CreateShaders();
    void CreateBindingLayout();
    void CreatePipeline();

    nvrhi::IDevice* m_Device;
    nvrhi::IFramebuffer* m_Framebuffer;
    nvrhi::CommandListHandle m_CommandList;

    nvrhi::BufferHandle m_VertexBuffer;
    nvrhi::ShaderHandle m_VertexShader;
    nvrhi::ShaderHandle m_PixelShader;
    nvrhi::BindingLayoutHandle m_BindingLayout;
    nvrhi::GraphicsPipelineHandle m_Pipeline;
};
// AABB definition
struct CubeAABB
{
    glm::vec3 min;
    glm::vec3 max;
};

// Camera constants for shaders
struct PipelineCameraSetting
{
    glm::mat4 viewProj;
};

class CubePipeline
{
public:
    CubePipeline(nvrhi::IDevice* device, nvrhi::IFramebuffer* framebuffer);
    ~CubePipeline();

    // Renders a vector of AABBs
    void Render(nvrhi::ICommandList* commandList, const std::vector<CubeAABB>& aabbs, const PipelineCameraSetting& cameraSetting, float width, float height);

private:
    void CreateConstantBuffer();
    void CreateShaders();
    void CreateBindingLayout();
    void CreatePipeline();

    void UploadAABBVertices(nvrhi::ICommandList* commandList, const std::vector<CubeAABB>& aabbs);

    nvrhi::IDevice* m_Device;
    nvrhi::IFramebuffer* m_Framebuffer;
    nvrhi::CommandListHandle m_CommandList;

    nvrhi::ShaderHandle m_VertexShader;
    nvrhi::ShaderHandle m_PixelShader;
    nvrhi::BufferHandle m_VertexBuffer;
    nvrhi::BufferHandle m_CameraCB;

    nvrhi::BindingLayoutHandle m_BindingLayout;
    nvrhi::BindingSetHandle m_BindingSet;
    nvrhi::GraphicsPipelineHandle m_Pipeline;
    
    uint32_t m_VertexCount = 0;
};