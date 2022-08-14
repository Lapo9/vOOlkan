#ifndef VULKAN_DESCRIPTORSETPOOL
#define VULKAN_DESCRIPTORSETPOOL

#include <vulkan/vulkan.h>

#include "VulkanException.h"
#include "LogicalDevice.h"


namespace Vulkan { class DescriptorSetPool; }

class Vulkan::DescriptorSetPool {
public:
	DescriptorSetPool(const LogicalDevice& virtualGpu, unsigned int size) : virtualGpu{ virtualGpu } {
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		poolSize.descriptorCount = static_cast<uint32_t>(size);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(size);

		if (VkResult result = vkCreateDescriptorPool(+virtualGpu, &poolInfo, nullptr, &descriptorPool); result != VK_SUCCESS) {
			throw VulkanException("Failed to create descriptor pool!", result);
		}
	}
	

	DescriptorSetPool(const DescriptorSetPool&) = delete;
	DescriptorSetPool& operator=(const DescriptorSetPool&) = delete;
	DescriptorSetPool(DescriptorSetPool&&) = delete;
	DescriptorSetPool& operator=(DescriptorSetPool&&) = delete;


	~DescriptorSetPool() {
		vkDestroyDescriptorPool(+virtualGpu, descriptorPool, nullptr);
	}


	const VkDescriptorPool& operator+() const {
		return descriptorPool;
	}

private:
	VkDescriptorPool descriptorPool;
	const LogicalDevice& virtualGpu;
};

#endif
