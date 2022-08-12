#ifndef VULKAN_DESCRIPTORSET
#define VULKAN_DESCRIPTORSET

#include <vulkan/vulkan.h>

#include "DecriptorSetPool.h"
#include "DescriptorSetLayout.h"
#include "LogicalDevice.h"
#include "VulkanException.h"


namespace Vulkan { class DescriptorSet; }

class Vulkan::DescriptorSet {
public:

	//FROMHERE this ctor should also get a UniformBuffer where to bind each binding of the layout linearly. There should also be another ctor which lets the user choose how to bind the bindings (so it should take one buffer and offset for each binding in the layout)
	DescriptorSet(const LogicalDevice& virtualGpu, const DescriptorSetPool& descriptorPool, const DescriptorSetLayout& layout) : descriptorSet{} {
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = +descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &+layout;

		if (VkResult result = vkAllocateDescriptorSets(+virtualGpu, &allocInfo, &descriptorSet); result != VK_SUCCESS) {
			throw VulkanException("Failed to allocate descriptor sets!", result);
		}
	}
	

	const VkDescriptorSet& operator&() {
		descriptorSet;
	}

private:
	VkDescriptorSet descriptorSet;

};

#endif
