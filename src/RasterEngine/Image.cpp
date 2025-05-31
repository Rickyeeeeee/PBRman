#include "Image.h"

Image::Image(uint32_t width, uint32_t height, nvrhi::DeviceHandle device, nvrhi::CommandListHandle commandList)
    : m_Width(width)
    , m_Height(height)
    , m_Device(device)
    , m_CommandList(commandList)
{
    nvrhi::TextureDesc textureDesc;
    textureDesc.width = width;
    textureDesc.height = height;
    textureDesc.format = nvrhi::Format::RGBA8_UNORM;
    textureDesc.debugName = "Image Texture";
    textureDesc.isRenderTarget = false;
    textureDesc.isUAV = false;
    textureDesc.initialState = nvrhi::ResourceStates::Common;
    textureDesc.keepInitialState = true;

    m_Texture = device->createTexture(textureDesc);
}

Image::~Image()
{

}

void Image::SetData(const void* data)
{
    m_CommandList->open();
    if (data)
    {
        m_CommandList->beginTrackingTextureState(m_Texture, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
        m_CommandList->writeTexture(m_Texture, 0, 0, data, m_Width * 4);
        m_CommandList->setTextureState(m_Texture, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
        m_CommandList->commitBarriers();
    }
    m_CommandList->close();
    m_Device->executeCommandList(m_CommandList);    
}


