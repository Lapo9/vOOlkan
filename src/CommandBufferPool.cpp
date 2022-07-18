#include "CommandBufferPool.h"
#include "LogicalDevice.h"
#include "Queue.h"
#include "QueueFamily.h"
#include "VulkanException.h"


Vulkan::CommandBufferPool::CommandBufferPool(const LogicalDevice& virtualGpu, QueueFamily queueFamily) : virtualGpu{ virtualGpu } {
	//struct to create a command buffer pool
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = virtualGpu[queueFamily].getFamilyIndex();

	if (VkResult result = vkCreateCommandPool(+virtualGpu, &poolInfo, nullptr, &commandBufferPool); result != VK_SUCCESS) {
		throw VulkanException("Failed to create command pool!", result);
	}
}



Vulkan::CommandBufferPool::~CommandBufferPool() {
	vkDestroyCommandPool(+virtualGpu, commandBufferPool, nullptr);
}



const VkCommandPool& Vulkan::CommandBufferPool::operator+() const {
	return commandBufferPool;
}
