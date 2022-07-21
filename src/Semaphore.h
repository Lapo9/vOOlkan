#ifndef VULKAN_SEMAPHORE
#define VULKAN_SEMAPHORE

#include <vulkan/vulkan.h>

#include "VulkanException.h"
#include "LogicalDevice.h"


namespace Vulkan::SynchronizationPrimitives { class Semaphore; }

class Vulkan::SynchronizationPrimitives::Semaphore {
public:
	
	Semaphore(const LogicalDevice& virtualGpu) : virtualGpu{ virtualGpu } {
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (VkResult result = vkCreateSemaphore(+virtualGpu, &semaphoreInfo, nullptr, &semaphore); result != VK_SUCCESS) {
			throw VulkanException{ "Failed to create semaphore", result };
		}
	}

	Semaphore(const Semaphore&) = delete;
	Semaphore(Semaphore&&) = default;
	Semaphore& operator=(const Semaphore&) = delete;
	Semaphore& operator=(Semaphore&&) = default;

	~Semaphore() {
		vkDestroySemaphore(+virtualGpu, semaphore, nullptr);
	}

	const VkSemaphore& operator+() const {
		return semaphore;
	}


private:
	VkSemaphore semaphore;
	const LogicalDevice& virtualGpu;
};

#endif