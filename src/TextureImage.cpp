#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vulkan/vulkan.h>

#include "TextureImage.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "StagingBuffer.h"
#include "CommandBuffer.h"
#include "CommandBufferPool.h"
#include "Barriers.h"
#include "Queue.h"



Vulkan::TextureImage::TextureImage(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, const CommandBufferPool& commandBufferPool, std::pair<unsigned int, unsigned int> resolution, std::string pathToTexture)
    : Image{ virtualGpu, realGpu, VK_FORMAT_R8G8B8A8_SRGB, resolution, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT },
    textureSampler{ virtualGpu, realGpu }
{
    //load image in CPU memory
    int width, height, channels;
    stbi_uc* pixels = stbi_load(pathToTexture.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (resolution != std::pair<unsigned int, unsigned int>{width, height}) {
        throw VulkanException{ "Width or height of the texture doesn't match width and height of the TextureImage object" };
    }
    if (!pixels) {
        throw VulkanException{ "Failed to load texture image!" };
    }
    VkDeviceSize imageSize = width * height * 4;

    //load image in a staging buffer, in order to copy it to the GPU next
    Buffers::StagingBuffer stagingBuffer{ virtualGpu, realGpu, imageSize };
    stagingBuffer.fillBuffer(pixels, imageSize);
    stbi_image_free(pixels); //release memory arei in CPU (the image is now in the buffer)

    //change the layout of the image in order to be able to receive data from a buffer
    transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandBufferPool);

    //fill the image with the data of the staging buffer
    fillImage(stagingBuffer, commandBufferPool);

    //change image layout to a layout suitable for sampling on the GPU
    transitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandBufferPool);

    //create the image view
    generateImageView("base", virtualGpu, VK_IMAGE_ASPECT_COLOR_BIT);
}



void Vulkan::TextureImage::transitionLayout(VkImageLayout newLayout, const CommandBufferPool& commandBufferPool) {
    auto transitionInfo = SynchronizationPrimitives::ImageMemoryBarrier::getTransitionInfo(layout, newLayout);

    SynchronizationPrimitives::ImageMemoryBarrier barrier{ *this, layout, newLayout };
    CommandBuffer commandBuffer{ *virtualGpu, commandBufferPool };
    commandBuffer.addCommand(vkCmdPipelineBarrier, transitionInfo.sourceStage, transitionInfo.destinationStage, 0, 0, nullptr, 0, nullptr, 1, &+barrier)
        .endCommand()
        .sendCommand((*virtualGpu)[QueueFamily::GRAPHICS]);
    vkQueueWaitIdle(+(*virtualGpu)[QueueFamily::GRAPHICS]); //TODO not best for performance

    layout = newLayout; //if everything went smoothly, change the layout in the object field
}



void Vulkan::TextureImage::fillImage(const Buffers::StagingBuffer& imageData, const CommandBufferPool& commandBufferPool) {
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { resolution.first, resolution.second, 1 };

    CommandBuffer commandBuffer{ *virtualGpu, commandBufferPool };
    commandBuffer.addCommand(vkCmdCopyBufferToImage, +imageData, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region)
        .endCommand()
        .sendCommand((*virtualGpu)[QueueFamily::GRAPHICS]);

    vkQueueWaitIdle(+(*virtualGpu)[QueueFamily::GRAPHICS]); //TODO not best for performance
}


const Vulkan::TextureSampler& Vulkan::TextureImage::getSampler() const {
    return textureSampler;
}