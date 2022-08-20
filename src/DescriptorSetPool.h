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
	 * @brief Creates a DescriptorSetPool object of the specified size. There is one pool of size size for each type.
	 * 
	 * @param virtualGpu The LogicalDevice.
	 * @param types The types of DescriptorSet that this pool can allocate.
	 * @param size The maximum number of DescriptorSet(s) that this pool can allocate per each type.
	 */
	template<std::same_as<VkDescriptorType>... DT>
	DescriptorSetPool(const LogicalDevice& virtualGpu, unsigned int size, DT... types) : virtualGpu{ virtualGpu } {
		//varaargs to vector
		std::vector<VkDescriptorType> typesArray;
		(typesArray.push_back(types), ...);

		//for each type create the info struct for each type
		std::vector<VkDescriptorPoolSize> typesInfo;
		for (const auto& poolType : typesArray) {
			VkDescriptorPoolSize poolSize;
			poolSize.type = poolType;
			poolSize.descriptorCount = static_cast<uint32_t>(size);
			typesInfo.push_back(poolSize);
		}

		//struct to create the pool
		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = typesInfo.size();
		poolInfo.pPoolSizes = typesInfo.data();
		poolInfo.maxSets = static_cast<uint32_t>(size);

		//actually create the pool
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
