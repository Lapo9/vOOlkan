#ifndef VULKAN_FENCE
#define VULKAN_FENCE

#include <vulkan/vulkan.h>
#include <iostream>

#include "VulkanException.h"
#include "LogicalDevice.h"


namespace Vulkan::SynchronizationPrimitives { class Fence; }

class Vulkan::SynchronizationPrimitives::Fence {
public:
	
	Fence(const LogicalDevice& virtualGpu) : virtualGpu{ virtualGpu } {
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (VkResult result = vkCreateFence(+virtualGpu, &fenceInfo, nullptr, &fence); result != VK_SUCCESS) {
			throw VulkanException{ "Failed to create fence", result };
		}

		std::cout << "\n+ Fence created";
	}

	Fence(const Fence&) = delete;
	Fence& operator=(const Fence&) = delete;


	Fence(Fence&& movedFrom) noexcept : fence{ movedFrom.fence }, virtualGpu{ movedFrom.virtualGpu } {
		movedFrom.fence = nullptr;
		std::cout << "\n> Fence moved";
	}

	//FIXTHIS implement this the right way
	Fence& operator=(Fence&&) = default;

	~Fence() {
		vkDestroyFence(+virtualGpu, fence, nullptr);
		std::cout << "\n- Fence destroyed";
	}

	const VkFence& operator+() const {
		return fence;
	}


private:
	VkFence fence;
	const LogicalDevice& virtualGpu;
};

#endif
