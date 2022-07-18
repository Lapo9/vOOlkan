#include "PipelineLayout.h"
#include "LogicalDevice.h"
#include "VulkanException.h"


Vulkan::PipelineOptions::PipelineLayout::PipelineLayout(const LogicalDevice& virtualGpu) : virtualGpu{ virtualGpu } {
	//TODO fill this appropriately when we have uniforms
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (VkResult result = vkCreatePipelineLayout(+virtualGpu, &pipelineLayoutInfo, nullptr, &pipelineLayout); result != VK_SUCCESS) {
		throw VulkanException("Failed to create pipeline layout!", result);
	}
}



Vulkan::PipelineOptions::PipelineLayout::~PipelineLayout() {
	vkDestroyPipelineLayout(+virtualGpu, pipelineLayout, nullptr);
}


const VkPipelineLayout Vulkan::PipelineOptions::PipelineLayout::operator+() const {
	return pipelineLayout;
}
