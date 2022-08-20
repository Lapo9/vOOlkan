#ifndef VULKAN_TEXTURESAMPLER
#define VULKAN_TEXTURESAMPLER

#include <vulkan/vulkan.h>

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "VulkanException.h"


namespace Vulkan { class TextureSampler; }

/**
 * @brief A TextureSampler is an object which is used to apply filters (anisotropic, interpolation, ...) and other techniques when accessing the textels of a texture image.
 */
class Vulkan::TextureSampler {
public:

	TextureSampler(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu) : virtualGpu{ &virtualGpu } {
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = realGpu.getProperties().limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (VkResult result = vkCreateSampler(+virtualGpu, &samplerInfo, nullptr, &textureSampler); result != VK_SUCCESS) {
			throw VulkanException{ "Failed to create texture sampler!", result };
		}
	}


	TextureSampler(const TextureSampler&) = delete;
	TextureSampler& operator=(const TextureSampler&) = delete;
	TextureSampler(TextureSampler&&) = delete;
	TextureSampler& operator=(TextureSampler&&) = delete;


	~TextureSampler() {
		vkDestroySampler(+*virtualGpu, textureSampler, nullptr);
	}


	const VkSampler& operator+() const {
		return textureSampler;
	}

private:

	VkSampler textureSampler;
	LogicalDevice const* virtualGpu;

};



#endif
