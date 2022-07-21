#ifndef VULKAN_FENCE
#define VULKAN_FENCE

#include <vulkan/vulkan.h>

#include "VulkanException.h"
#include "LogicalDevice.h"


namespace Vulkan::SynchronizationPrimitives { class Fence; }

class Vulkan::SynchronizationPrimitives::Fence {
public:
	
	Fence(const LogicalDevice& virtualGpu) : virtualGpu{ virtualGpu } {
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		if (VkResult result = vkCreateFence(+virtualGpu, &fenceInfo, nullptr, &fence); result != VK_SUCCESS) {
			throw VulkanException{ "Failed to create fence", result };
		}
	}

	Fence(const Fence&) = delete;
	Fence(Fence&&) = default;
	Fence& operator=(const Fence&) = delete;
	Fence& operator=(Fence&&) = default;

	~Fence() {
		vkDestroyFence(+virtualGpu, fence, nullptr);
	}

	const VkFence& operator+() const {
		return fence;
	}


private:
	VkFence fence;
	const LogicalDevice& virtualGpu;
};

#endif
