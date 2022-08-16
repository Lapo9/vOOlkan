#ifndef VULKAN_DESCRIPTORSETPOOL
#define VULKAN_DESCRIPTORSETPOOL

#include <vulkan/vulkan.h>

#include "VulkanException.h"
#include "LogicalDevice.h"


namespace Vulkan { class DescriptorSetPool; }


/**
 * @brief Class used to allocate DescriptorSet(s).
 */
class Vulkan::DescriptorSetPool {
public:

	/**
	 * @brief Creates a DescriptorSetPool object of the specified size.
	 * 
	 * @param virtualGpu The LogicalDevice.
	 * @param type The type of DescriptorSet(s) that this pool can allocate.
	 * @param size The maximum number of DescriptorSet(s) that this pool can allocate.
	 */
	DescriptorSetPool(const LogicalDevice& virtualGpu, VkDescriptorType type, unsigned int size) : virtualGpu{ virtualGpu } {
		VkDescriptorPoolSize poolSize{};
		poolSize.type = type;
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
