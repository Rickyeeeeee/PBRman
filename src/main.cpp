#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <nvrhi/nvrhi.h>
#include <nvrhi/d3d12.h>
#include <nvrhi/validation.h>
#include <nvrhi/utils.h>

#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>

#define GLM_FORCE_INTRINSICS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include <core/imgui_nvrhi.h>
#include <core/imgui_impl_glfw.h>

void GetHardwareAdapter(IDXGIFactory1 *pFactory, IDXGIAdapter1 **ppAdapter, bool requestHighPerformanceAdapter = true);
int InitWindow();
void InitDevice();
void CreateSwapChainRenderTargets();
void CreateAssets();
void InitImgui();

// Helper Functions
void ThrowIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        std::cerr << "HRESULT failed: 0x" << std::hex << hr << std::endl; // More info
        throw std::runtime_error("HRESULT failed");
    }
}

// Constants
const UINT windowWidth = 800;
const UINT windowHeight = 600;
const UINT FrameCount = 2;

bool enableNVRHIValidation = true;

// Window objects
HWND hwnd = nullptr;
GLFWwindow* window = nullptr;

// Device objects
nvrhi::RefCountPtr<IDXGIFactory4> factory;
nvrhi::RefCountPtr<IDXGIAdapter1> hardwareAdapter;
nvrhi::RefCountPtr<ID3D12Device> device;
nvrhi::RefCountPtr<IDXGISwapChain3> swapChain;
nvrhi::RefCountPtr<ID3D12CommandQueue> commandQueue;
nvrhi::DeviceHandle nvrhiDevice;

// Swapchain render targets
nvrhi::RefCountPtr<ID3D12Resource> swapChainBuffers[FrameCount];
nvrhi::TextureHandle swapChainBufferHandles[FrameCount];
nvrhi::RefCountPtr<ID3D12Fence> frameFence;
uint32_t frameFenceValue[FrameCount] = { 0 };
uint32_t fenceValue = 0;
HANDLE frameFenceEvent;
nvrhi::FramebufferHandle swapChainFrameBufferHandles[FrameCount];

// Application objects
nvrhi::CommandListHandle commandList;

// ImGui objects
std::shared_ptr<ImGui_NVRHI> nvrhiImgui;

// Vertex structure
struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

// Cube data
Vertex cubeVertices[] = {
    {{-1,-1,-1}, {1,0,0}}, {{-1, 1,-1}, {0,1,0}}, {{1, 1,-1}, {0,0,1}}, {{1,-1,-1}, {1,1,0}},
    {{-1,-1, 1}, {1,0,1}}, {{-1, 1, 1}, {0,1,1}}, {{1, 1, 1}, {1,1,1}}, {{1,-1, 1}, {0,0,0}},
};

uint16_t cubeIndices[] = {
    0,1,2, 0,2,3,  4,6,5, 4,7,6,
    4,5,1, 4,1,0,  3,2,6, 3,6,7,
    1,5,6, 1,6,2,  4,0,3, 4,3,7,
};

int main() {

    glm::vec4 color = { 0.5f, 0.6f, 0.2f, 1.0f };
    std::cout << "is aligned: " << color.value << std::endl;

    auto retVal = InitWindow();
    if (retVal < 0)
        return -1;

    InitDevice();
    CreateSwapChainRenderTargets();
    CreateAssets();
    InitImgui();

    ImGuiIO& io = ImGui::GetIO(); (void)io;

   // Our state
    bool show_demo_window = true;
    bool show_another_window = false;

    // TODO: Haven't implemented resizing yet
    while(!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        // ImGui_ImplWin32_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
                ImGui::End();
        }
        ImGui::Render();
            
        // Get the current back buffer
        auto backBufferIndex = swapChain->GetCurrentBackBufferIndex();
        auto backFrameBuffer = swapChainFrameBufferHandles[backBufferIndex];
        
        // Rendering
        commandList->open();
        commandList->setResourceStatesForFramebuffer(backFrameBuffer);
        nvrhi::utils::ClearColorAttachment(
            commandList, 
            backFrameBuffer, 
            0,
            nvrhi::Color(0.5f, 0.6f, 0.2f, 1.0f));
        commandList->close();
        nvrhiDevice->executeCommandList(commandList);

        nvrhiImgui->render(backFrameBuffer);

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
        
        // Present & Swap Buffers
        ThrowIfFailed(swapChain->Present(1, 0));
        frameFenceValue[backBufferIndex] = fenceValue++;
        ThrowIfFailed(commandQueue->Signal(frameFence.Get(), frameFenceValue[backBufferIndex]));
        backBufferIndex = swapChain->GetCurrentBackBufferIndex();

        // Wait for the next back buffer to be ready
        if (frameFence->GetCompletedValue() < frameFenceValue[backBufferIndex])
        {
            ThrowIfFailed(frameFence->SetEventOnCompletion(frameFenceValue[backBufferIndex], frameFenceEvent));
            ::WaitForSingleObject(frameFenceEvent, INFINITE);
        }


    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void InitDevice()
{
// DX12 Initialization
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    {
        nvrhi::RefCountPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            std::cout << "Debug Layer Enabled" << std::endl;
            debugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
    
    GetHardwareAdapter(factory.Get(), &hardwareAdapter);

    ThrowIfFailed(D3D12CreateDevice(
        hardwareAdapter.Get(),
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&device)
    ));

    D3D12_COMMAND_QUEUE_DESC qdesc = {};
    qdesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    qdesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    qdesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    ThrowIfFailed(device->CreateCommandQueue(&qdesc, IID_PPV_ARGS(&commandQueue)));

    nvrhi::d3d12::DeviceDesc deviceDesc;
    deviceDesc.pDevice = device.Get();
    deviceDesc.pGraphicsCommandQueue = commandQueue.Get();

    nvrhiDevice = nvrhi::d3d12::createDevice(deviceDesc);
    nvrhiDevice = nvrhi::validation::createValidationLayer(nvrhiDevice);

    DXGI_SWAP_CHAIN_DESC1 scDesc = {};
    scDesc.BufferCount = FrameCount; // Use FrameCount
    scDesc.Width = windowWidth;
    scDesc.Height = windowHeight;
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.SampleDesc.Count = 1;

    nvrhi::RefCountPtr<IDXGISwapChain1> sc1;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &scDesc, nullptr, nullptr, &sc1));
    ThrowIfFailed(sc1->QueryInterface(IID_PPV_ARGS(&swapChain)));

    // Create a fence for the swap chain
    ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frameFence)));

    // Create an event handle to use for frame synchronization
    frameFenceEvent = CreateEvent(nullptr, false, true, nullptr);
}

void CreateSwapChainRenderTargets()
{
    for(UINT n = 0; n < FrameCount; n++)
    {
        ThrowIfFailed(swapChain->GetBuffer(n, IID_PPV_ARGS(&swapChainBuffers[n])));

        nvrhi::TextureDesc textureDesc;
        textureDesc.width = windowWidth;
        textureDesc.height = windowHeight;
        textureDesc.sampleCount = 1;
        textureDesc.sampleQuality = 0;
        textureDesc.format = nvrhi::Format::RGBA8_UNORM;
        textureDesc.debugName = "SwapChainBuffer";
        textureDesc.isRenderTarget = true;
        textureDesc.isUAV = false;
        textureDesc.initialState = nvrhi::ResourceStates::Present;
        textureDesc.keepInitialState = true;

        swapChainBufferHandles[n] = nvrhiDevice->createHandleForNativeTexture(
            nvrhi::ObjectTypes::D3D12_Resource, 
            nvrhi::Object(swapChainBuffers[n]), 
            textureDesc);

        swapChainFrameBufferHandles[n] = nvrhiDevice->createFramebuffer(
            nvrhi::FramebufferDesc().addColorAttachment(swapChainBufferHandles[n])
        );
    }
}

void CreateAssets()
{
    commandList = nvrhiDevice->createCommandList();
}

void InitImgui() 
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    io.ConfigViewportsNoAutoMerge = true;
    io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 0.5f;
    }
    ImGui_ImplGlfw_InitForOther(window, true);
    nvrhiImgui = std::make_shared<ImGui_NVRHI>();
    nvrhiImgui->init(nvrhiDevice);
    nvrhiImgui->updateFontTexture();
}

int InitWindow()
{
    // Windows init
    glfwInit();
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Disable OpenGL context

    window = glfwCreateWindow(windowWidth, windowHeight, "PBRMan", nullptr, nullptr);

    hwnd = glfwGetWin32Window(window);

    if (!hwnd)
    {
        std::cerr << "Failed to get window handle" << std::endl;
        return -1;
    }

    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        if (width > 0 && height > 0)
        {
            
            nvrhiDevice->waitForIdle();
            nvrhiDevice->runGarbageCollection();
            SetEvent(frameFenceEvent);
            
            for (UINT n = 0; n < FrameCount; n++)
            {
                swapChainBuffers[n].Reset();
                swapChainBufferHandles[n].Reset();
                swapChainFrameBufferHandles[n].Reset();
            }

            DXGI_SWAP_CHAIN_DESC1 scDesc = {};
            scDesc.BufferCount = FrameCount; // Use FrameCount
            scDesc.Width = windowWidth;
            scDesc.Height = windowHeight;
            scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            scDesc.SampleDesc.Count = 1;

            ThrowIfFailed(swapChain->ResizeBuffers(
                FrameCount,
                width,
                height,
                DXGI_FORMAT_R8G8B8A8_UNORM,
                DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
            ));

            CreateSwapChainRenderTargets();

            nvrhiImgui->backbufferResizing();

            ImGui::SetNextWindowSize(ImVec2((float)width, (float)height));
        }
    });
    return 1;
}

static void GetHardwareAdapter(
    IDXGIFactory1* pFactory,
    IDXGIAdapter1** ppAdapter,
    bool requestHighPerformanceAdapter) {
    *ppAdapter = nullptr;
    nvrhi::RefCountPtr<IDXGIAdapter1> adapter;

    nvrhi::RefCountPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6)))) {
        for (UINT adapterIndex = 0;
            SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                requestHighPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                IID_PPV_ARGS(&adapter)));
                ++adapterIndex) {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
                break;
            }
        }
    }

    if (!adapter) {
        for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex) {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
                break;
            }
        }
    }

    *ppAdapter = adapter.Detach();
    if (!*ppAdapter) {
        throw std::runtime_error("No suitable Direct3D 12 adapter found.");
    }
}
