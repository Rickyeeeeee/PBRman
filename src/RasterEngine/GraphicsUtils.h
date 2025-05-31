#pragma once

#include <d3dcompiler.h>
#include <nvrhi/nvrhi.h>
#include <wrl/client.h>
#include <stdexcept>
#include <string>

nvrhi::ShaderHandle LoadShader(nvrhi::DeviceHandle device, const std::wstring& filename, nvrhi::ShaderType type, const std::string& entryPoint = "main")
{
    const char* target = nullptr;
    switch (type)
    {
        case nvrhi::ShaderType::Vertex:   target = "vs_5_1"; break;
        case nvrhi::ShaderType::Pixel:    target = "ps_5_1"; break;
        case nvrhi::ShaderType::Compute:  target = "cs_5_1"; break;
        default: throw std::runtime_error("Unsupported shader type");
    }

    Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompileFromFile(
        filename.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint.c_str(),
        target,
        D3DCOMPILE_OPTIMIZATION_LEVEL3,
        0,
        &shaderBlob,
        &errorBlob
    );

    if (FAILED(hr))
    {
        std::string errorMessage = "Shader compilation failed: ";
        if (errorBlob)
            errorMessage += static_cast<const char*>(errorBlob->GetBufferPointer());
        throw std::runtime_error(errorMessage);
    }

    nvrhi::ShaderHandle shader = device->createShader(
        nvrhi::ShaderDesc().setShaderType(type),
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize()
    );

    return shader;
}
