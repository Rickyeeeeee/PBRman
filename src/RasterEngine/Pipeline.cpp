#include "Pipeline.h"
#include "core/Core.h"
#include "GraphicsUtils.h"
#include <nvrhi/utils.h>
#include <string>

// Vertex structure
struct Vertex
{
    float Position[2];
    float TexCoord[2];
};

// Fullscreen quad vertices
static const Vertex quadVertices[] = {
    { {-1.0f, -1.0f }, {0.0f, 1.0f} },
    { {-1.0f,  1.0f }, {0.0f, 0.0f} },
    { { 1.0f, -1.0f }, {1.0f, 1.0f} },
    { { 1.0f,  1.0f }, {1.0f, 0.0f} },
};

FullscreenQuadPipeline::FullscreenQuadPipeline(nvrhi::IDevice* device, nvrhi::IFramebuffer* framebuffer)
    : m_Device(device), m_Framebuffer(framebuffer)
{

    m_CommandList = m_Device->createCommandList();

    CreateVertexBuffer();
    CreateShaders();
    CreateBindingLayout();
    CreatePipeline();
}

FullscreenQuadPipeline::~FullscreenQuadPipeline()
{
    // Resources are released automatically by their handles
}

void FullscreenQuadPipeline::CreateVertexBuffer()
{
    nvrhi::BufferDesc vbDesc = nvrhi::BufferDesc()
        .setByteSize(sizeof(quadVertices))
        .setIsVertexBuffer(true)
        .setInitialState(nvrhi::ResourceStates::VertexBuffer)
        .setKeepInitialState(true)
        .setDebugName("FullscreenQuadVB")
    ;
    
    m_VertexBuffer = m_Device->createBuffer(vbDesc);

    m_CommandList->open();

    m_CommandList->writeBuffer(m_VertexBuffer, quadVertices, sizeof(quadVertices));

    m_CommandList->close();

    m_Device->executeCommandList(m_CommandList);

    m_Device->waitForIdle();
}

void FullscreenQuadPipeline::CreateShaders()
{
    // Load and compile shaders
    m_VertexShader = LoadShader(m_Device, std::wstring(LRUNTIME_DIRECTORY) + L"assets/shaders/fullscreen_quad.hlsl", nvrhi::ShaderType::Vertex, "VSMain");
    m_PixelShader  = LoadShader(m_Device, std::wstring(LRUNTIME_DIRECTORY) + L"assets/shaders/fullscreen_quad.hlsl", nvrhi::ShaderType::Pixel, "PSMain");
}

void FullscreenQuadPipeline::CreateBindingLayout()
{
    nvrhi::BindingLayoutDesc bindingLayoutDesc;
    bindingLayoutDesc.visibility = nvrhi::ShaderType::Pixel;
    bindingLayoutDesc.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0)); // t0
    bindingLayoutDesc.addItem(nvrhi::BindingLayoutItem::Sampler(0));     // s0

    m_BindingLayout = m_Device->createBindingLayout(bindingLayoutDesc);
}

void FullscreenQuadPipeline::CreatePipeline()
{
    nvrhi::VertexAttributeDesc attributes[] = {
        nvrhi::VertexAttributeDesc()
            .setName("POSITION")
            .setFormat(nvrhi::Format::RG32_FLOAT)
            .setOffset(offsetof(Vertex, Position))
            .setElementStride(sizeof(Vertex)),
        nvrhi::VertexAttributeDesc()
            .setName("TEXCOORD")
            .setFormat(nvrhi::Format::RG32_FLOAT)
            .setOffset(offsetof(Vertex, TexCoord))
            .setElementStride(sizeof(Vertex)),
    };

    nvrhi::InputLayoutHandle inputLayout = m_Device->createInputLayout(
        attributes, uint32_t(std::size(attributes)), m_VertexShader);

    nvrhi::GraphicsPipelineDesc pipelineDesc = nvrhi::GraphicsPipelineDesc()
        .setPrimType(nvrhi::PrimitiveType::TriangleStrip)
        .setVertexShader(m_VertexShader)
        .setPixelShader(m_PixelShader)
        .addBindingLayout(m_BindingLayout)
        .setInputLayout(inputLayout);

    m_Pipeline = m_Device->createGraphicsPipeline(pipelineDesc, m_Framebuffer);
}

void FullscreenQuadPipeline::Render(nvrhi::ICommandList* commandList, nvrhi::ITexture* texture, nvrhi::ISampler* sampler, float width, float height)
{
    // Create binding set
    nvrhi::BindingSetDesc bindingSetDesc;
    bindingSetDesc.addItem(nvrhi::BindingSetItem::Texture_SRV(0, texture));
    bindingSetDesc.addItem(nvrhi::BindingSetItem::Sampler(0, sampler));

    nvrhi::BindingSetHandle bindingSet = m_Device->createBindingSet(bindingSetDesc, m_BindingLayout);

    // Set graphics state
    nvrhi::GraphicsState state = nvrhi::GraphicsState()
        .setPipeline(m_Pipeline)
        .setFramebuffer(m_Framebuffer)
        .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(width, height)))
        .addBindingSet(bindingSet)
        .addVertexBuffer(nvrhi::VertexBufferBinding().setBuffer(m_VertexBuffer));
    commandList->setGraphicsState(state);
    commandList->setResourceStatesForFramebuffer(m_Framebuffer);
    nvrhi::utils::ClearColorAttachment(commandList, m_Framebuffer, 0, nvrhi::Color{ 0.0f, 0.0f, 0.0f, 1.0f });
    nvrhi::utils::ClearDepthStencilAttachment(commandList, m_Framebuffer, 1.0f, 0);    

    auto drawArguements = nvrhi::DrawArguments()
        .setVertexCount(std::size(quadVertices));
    commandList->draw(drawArguements);
}


using namespace DirectX;

struct CubeVertex
{
    glm::vec3 position;
};

static const uint32_t kVerticesPerBox = 24;

static const uint32_t boxEdgeIndices[24] = {
    0,1, 1,3, 3,2, 2,0, // Bottom face
    4,5, 5,7, 7,6, 6,4, // Top face
    0,4, 1,5, 2,6, 3,7  // Vertical edges
};

static void GenerateBoxVertices(const CubeAABB& aabb, std::vector<CubeVertex>& outVertices)
{
    const glm::vec3& min = aabb.min;
    const glm::vec3& max = aabb.max;

    glm::vec3 corners[8] = {
        { min.x, min.y, min.z },
        { max.x, min.y, min.z },
        { min.x, min.y, max.z },
        { max.x, min.y, max.z },
        { min.x, max.y, min.z },
        { max.x, max.y, min.z },
        { min.x, max.y, max.z },
        { max.x, max.y, max.z },
    };

    for (int i = 0; i < 24; i++)
    {
        CubeVertex v;
        v.position = corners[boxEdgeIndices[i]];
        outVertices.push_back(v);
    }
}

CubePipeline::CubePipeline(nvrhi::IDevice* device, nvrhi::IFramebuffer* framebuffer)
    : m_Device(device), m_Framebuffer(framebuffer)
{
    m_CommandList = m_Device->createCommandList();

    CreateConstantBuffer();
    CreateShaders();
    CreateBindingLayout();
    CreatePipeline();
}

CubePipeline::~CubePipeline() {}

void CubePipeline::CreateShaders()
{
    m_VertexShader = LoadShader(m_Device, std::wstring(LRUNTIME_DIRECTORY) + L"assets/shaders/debug_cube.hlsl", nvrhi::ShaderType::Vertex, "VSMain");
    m_PixelShader  = LoadShader(m_Device, std::wstring(LRUNTIME_DIRECTORY) + L"assets/shaders/debug_cube.hlsl", nvrhi::ShaderType::Pixel, "PSMain");
}

void CubePipeline::CreateBindingLayout()
{
    nvrhi::BindingLayoutDesc layoutDesc;
    layoutDesc.visibility = nvrhi::ShaderType::Vertex;
    layoutDesc.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(0)); // b0

    m_BindingLayout = m_Device->createBindingLayout(layoutDesc);

    nvrhi::BindingSetDesc setDesc;
    setDesc.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, m_CameraCB));
    m_BindingSet = m_Device->createBindingSet(setDesc, m_BindingLayout);
}

void CubePipeline::CreatePipeline()
{
    nvrhi::VertexAttributeDesc attributes[] = {
        nvrhi::VertexAttributeDesc()
            .setName("POSITION")
            .setFormat(nvrhi::Format::RGB32_FLOAT)
            .setOffset(0)
            .setElementStride(sizeof(CubeVertex))
    };

    auto inputLayout = m_Device->createInputLayout(attributes, 1, m_VertexShader);

    nvrhi::GraphicsPipelineDesc pipelineDesc;
    pipelineDesc.primType = nvrhi::PrimitiveType::LineList;
    pipelineDesc.VS = m_VertexShader;
    pipelineDesc.PS = m_PixelShader;
    pipelineDesc.bindingLayouts = { m_BindingLayout };
    pipelineDesc.inputLayout = inputLayout;
    pipelineDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
    pipelineDesc.renderState.depthStencilState.depthTestEnable = false;

    m_Pipeline = m_Device->createGraphicsPipeline(pipelineDesc, m_Framebuffer);
}

void CubePipeline::UploadAABBVertices(nvrhi::ICommandList* commandList, const std::vector<CubeAABB>& aabbs)
{
    std::vector<CubeVertex> vertices;
    vertices.reserve(aabbs.size() * kVerticesPerBox);

    for (const CubeAABB& box : aabbs)
        GenerateBoxVertices(box, vertices);

    m_VertexCount = static_cast<uint32_t>(vertices.size());

    if (!m_VertexBuffer || m_VertexBuffer->getDesc().byteSize < vertices.size() * sizeof(CubeVertex))
    {
        nvrhi::BufferDesc desc;
        desc.byteSize = sizeof(CubeVertex) * vertices.size();
        desc.isVertexBuffer = true;
        desc.debugName = "CubeVB";
        desc.isVolatile = false;
        desc.initialState = nvrhi::ResourceStates::VertexBuffer;
        desc.keepInitialState = true;
        m_VertexBuffer = m_Device->createBuffer(desc);
    }

    commandList->beginTrackingBufferState(m_VertexBuffer, nvrhi::ResourceStates::VertexBuffer);
    commandList->writeBuffer(m_VertexBuffer, vertices.data(), sizeof(CubeVertex) * vertices.size());
}


void CubePipeline::Render(nvrhi::ICommandList* commandList, const std::vector<CubeAABB>& aabbs, const PipelineCameraSetting& cameraSetting, float width, float height)
{
    if (aabbs.empty()) return;

    UploadAABBVertices(commandList, aabbs);

    // Upload camera matrix
    commandList->beginTrackingBufferState(m_CameraCB, nvrhi::ResourceStates::ConstantBuffer);
    commandList->writeBuffer(m_CameraCB, &cameraSetting, sizeof(cameraSetting));

    commandList->setGraphicsState(nvrhi::GraphicsState()
        .setPipeline(m_Pipeline)
        .setFramebuffer(m_Framebuffer)
        .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(width, height)))
        .addVertexBuffer(nvrhi::VertexBufferBinding().setBuffer(m_VertexBuffer))
        .addBindingSet(m_BindingSet)
    );

    commandList->draw(nvrhi::DrawArguments().setVertexCount(m_VertexCount));
}

void CubePipeline::CreateConstantBuffer()
{
    // Create constant buffer for camera
    nvrhi::BufferDesc cbDesc;
    cbDesc.byteSize = sizeof(PipelineCameraSetting);
    cbDesc.isConstantBuffer = true;
    cbDesc.debugName = "CameraCB";
    cbDesc.isVolatile = false;
    cbDesc.canHaveUAVs = false;
    cbDesc.initialState = nvrhi::ResourceStates::ConstantBuffer;
    cbDesc.keepInitialState = true;

    m_CameraCB = m_Device->createBuffer(cbDesc);
    
}
