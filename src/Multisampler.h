#ifndef	VULKAN_MULTISAMPLER
#define	VULKAN_MULTISAMPLER

#include <vulkan/vulkan.h>

namespace Vulkan::PipelineOptions { class Multisampler; }


//TODO this class must be rewritten when we'll use the multisampler: https://vulkan-tutorial.com/Multisampling
class Vulkan::PipelineOptions::Multisampler {
public:
	Multisampler() : multisampler{}  {
		multisampler.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampler.sampleShadingEnable = VK_FALSE;
		multisampler.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampler.minSampleShading = 1.0f;
		multisampler.pSampleMask = nullptr;
		multisampler.alphaToCoverageEnable = VK_FALSE;
		multisampler.alphaToOneEnable = VK_FALSE;
	}


	Multisampler(VkPipelineMultisampleStateCreateInfo baseMultisampler) : multisampler{ baseMultisampler } {}


	const VkPipelineMultisampleStateCreateInfo& operator+() const {
		return multisampler;
	}


private:
	VkPipelineMultisampleStateCreateInfo multisampler;
};

#endif
