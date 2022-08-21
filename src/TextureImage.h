#ifndef VULKAN_TEXTUREIMAGE
#define VULKAN_TEXTUREIMAGE

#include <vulkan/vulkan.h>
#include <string>
#include <utility>

#include "TextureSampler.h"
#include "Image.h"


namespace Vulkan {
    class TextureImage; class LogicalDevice; class PhysicalDevice; class CommandBufferPool; class TextureSampler;
    namespace Buffers { class StagingBuffer; }
}


class Vulkan::TextureImage : public Vulkan::Image {
public:
    TextureImage(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, const CommandBufferPool& commandBufferPool, std::pair<unsigned int, unsigned int> resolution, std::string pathToTexture);

	TextureImage(const TextureImage&) = delete;
	TextureImage& operator=(const TextureImage&) = delete;
	TextureImage(TextureImage&&) = default;
	TextureImage& operator=(TextureImage&& movedFrom) = default;

	~TextureImage() = default;



    /**
     * @brief Changes the layout of the image.
     * 
     * @param newLayout The New layout to apply.
     * @param commandBufferPool From where to allocate the command buffer used to perform the transition.
     */
    void transitionLayout(VkImageLayout newLayout, const CommandBufferPool& commandBufferPool);


    /**
     * @brief Fills the image with the data stored in the buffer.
     * 
     * @param imageData Data to load to the image (generally textels).
     * @param commandBufferPool From where to allocate the command buffer used to perform the copy.
     */
    void fillImage(const Buffers::StagingBuffer& imageData, const CommandBufferPool& commandBufferPool);


    const TextureSampler& getSampler() const;

private:
    TextureSampler textureSampler; //how the image is filtered before accessing (e.g. anisotropic)

};



#endif