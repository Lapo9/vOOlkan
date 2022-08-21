#ifndef VULKAN_PIPELINELAYOUT
#define VULKAN_PIPELINELAYOUT

#include <vulkan/vulkan.h>
#include <concepts>

#include "PipelineLayout.h"
#include "LogicalDevice.h"
#include "VulkanException.h"
#include "Set.h"


namespace Vulkan::PipelineOptions { class PipelineLayout; }


/**
 * @brief This objects is a reference to all of the resources (e.g. uniforms) that the pipeline can access.
 */
class Vulkan::PipelineOptions::PipelineLayout {
public:

	template<typename... S> requires (std::derived_from<S, Set> && ...)
	PipelineLayout(const LogicalDevice& virtualGpu, const S&... sets) : virtualGpu{ virtualGpu } {
		//put the descriptor set layouts in a vector
		std::vector<VkDescriptorSetLayout> rawLayouts;
		(rawLayouts.emplace_back(+sets), ...);
		
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = rawLayouts.size();
		pipelineLayoutInfo.pSetLayouts = rawLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (VkResult result = vkCreatePipelineLayout(+virtualGpu, &pipelineLayoutInfo, nullptr, &pipelineLayout); result != VK_SUCCESS) {
			throw VulkanException("Failed to create pipeline layout!", result);
		}
	}

	PipelineLayout(const PipelineLayout&) = delete;
	PipelineLayout(PipelineLayout&&) = delete;
	PipelineLayout& operator=(const PipelineLayout&) = delete;
	PipelineLayout& operator=(PipelineLayout&&) = delete;

	~PipelineLayout() {
		vkDestroyPipelineLayout(+virtualGpu, pipelineLayout, nullptr);
	}


	const VkPipelineLayout operator+() const {
		return pipelineLayout;
	}


private:
	VkPipelineLayout pipelineLayout;
	const LogicalDevice& virtualGpu;
};


#endif
