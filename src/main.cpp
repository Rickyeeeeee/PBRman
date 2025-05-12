#include <iostream>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <nvrhi/nvrhi.h>
#include <nvrhi/d3d12.h>
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>

void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter=true);

// Helper Functions
void ThrowIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        std::cerr << "HRESULT failed: 0x" << std::hex << hr << std::endl; // More info
        throw std::runtime_error("HRESULT failed");
    }
}

// Constants
const UINT Width = 800;
const UINT Height = 600;
const UINT FrameCount = 2;

// Native objects
HWND hwnd = nullptr;
Microsoft::WRL::ComPtr<ID3D12Device> device;
Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;

// nvrhi objects
nvrhi::DeviceHandle nvrhiDevice;
bool enableNVRHIValidation = true;
nvrhi::TextureHandle swapChainTextures[FrameCount];

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Disable OpenGL context

    GLFWwindow* window = glfwCreateWindow(800, 600, "PBRMan", nullptr, nullptr);

    HWND hwnd = glfwGetWin32Window(window);

    if (!hwnd) {
        std::cerr << "Failed to get window handle" << std::endl;
        return -1;
    }

        UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            std::cout << "Debug Layer Enabled" << std::endl;
            debugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
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

    nvrhi::DeviceHandle nvrhiDevice = nvrhi::d3d12::createDevice(deviceDesc);

    DXGI_SWAP_CHAIN_DESC1 scDesc = {};
    scDesc.BufferCount = FrameCount; // Use FrameCount
    scDesc.Width = Width;
    scDesc.Height = Height;
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.SampleDesc.Count = 1;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> sc1;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &scDesc, nullptr, nullptr, &sc1));
    ThrowIfFailed(sc1.As(&swapChain));


    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    // Cleanup

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

static void GetHardwareAdapter(
    IDXGIFactory1* pFactory,
    IDXGIAdapter1** ppAdapter,
    bool requestHighPerformanceAdapter) {
    *ppAdapter = nullptr;
    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

    Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;
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
