#ifndef VULKAN_TEXTUREIMAGE
#define VULKAN_TEXTUREIMAGE

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vulkan/vulkan.h>

#include "Image.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "StagingBuffer.h"
#include "CommandBuffer.h"
#include "CommandBufferPool.h"


namespace Vulkan { class TextureImage; }

class Vulkan::TextureImage : public Vulkan::Image {
public:
	TextureImage(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, const CommandBufferPool& commandBufferPool, std::pair<unsigned int, unsigned int> resolution)
		: Image{ virtualGpu, realGpu, VK_FORMAT_R8G8B8A8_SRGB, resolution, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT }
	{
        //load image in CPU memory
        int width, height, channels;
        stbi_uc* pixels = stbi_load("textures/texture.jpg", &width, &height, &channels, STBI_rgb_alpha);
        if (resolution != std::pair<unsigned int, unsigned int>{width, height}) {
            throw VulkanException{ "Width or height of the texture doesn't match width and height of the TextureImage object" };
        }
        if (!pixels) {
            throw VulkanException{"Failed to load texture image!"};
        }
        VkDeviceSize imageSize = width * height * 4;

        //load image in a staging buffer, in order to copy it to the GPU next
        Buffers::StagingBuffer stagingBuffer{ virtualGpu, realGpu, imageSize };
        stagingBuffer.fillBuffer(pixels, imageSize);
        stbi_image_free(pixels); //release memory arei in CPU (the image is now in the buffer)

        //create command buffer to copy the staging buffer to the GPU
        CommandBuffer copyToGpu{ virtualGpu, commandBufferPool };
        copyToGpu.addCommands(
            //FROMHERE probably add a ctor to CommandBuffer to create a single use command buffer
        )




		generateImageView("base", virtualGpu, VK_IMAGE_ASPECT_DEPTH_BIT); //FIXTHIS is this necessary?
	}

	TextureImage(const TextureImage&) = delete;
	TextureImage& operator=(const TextureImage&) = delete;
	TextureImage(TextureImage&&) = default;
	TextureImage& operator=(TextureImage&& movedFrom) = default;

	~TextureImage() = default;
};



#endif