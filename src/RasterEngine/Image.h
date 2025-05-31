#pragma once

#include <nvrhi/nvrhi.h>
#include <string>

// RGBA
class Image
{
public:
    // Image(nvrhi::DeviceHandle device, nvrhi::CommandListHandle commandList, const std::string& filePath);
    Image(uint32_t width, uint32_t height, nvrhi::DeviceHandle device, nvrhi::CommandListHandle commandList);
    ~Image();

    void SetData(const void* data);

    inline nvrhi::TextureHandle GetTexture() const { return m_Texture; }
    inline nvrhi::CommandListHandle GetCommandList() const { return m_CommandList; }
    inline uint32_t GetWidth() const { return m_Width; }
    inline uint32_t GetHeight() const { return m_Height; }
    inline const std::string& GetFilePath() const { return m_FilePath; }

private:
    uint32_t m_Width;
    uint32_t m_Height;

    nvrhi::DeviceHandle m_Device;
    nvrhi::TextureHandle m_Texture;
    nvrhi::CommandListHandle m_CommandList;

    std::string m_FilePath;
};