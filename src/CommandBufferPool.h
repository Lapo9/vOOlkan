#ifndef VULKAN_COMMANDBUFFERPOOL
#define VULKAN_COMMANDBUFFERPOOL

#include <vulkan/vulkan.h>

#include "QueueFamily.h"


namespace Vulkan { class CommandBufferPool; class LogicalDevice; }

/**
 * @brief A CommandBufferPool is an object which is used to allocate CommandBuffers.
 */
class Vulkan::CommandBufferPool {
public:

	/**
	 * @brief Creates a command buffer pool for the specified queue family.
	 * 
	 * @param virtualGpu The LogicalDevice where the CommandBuffer(s) originating from this pool will send their commands.
	 * @param queueFamily The QueueFamily where the CommandBuffer(s) originating from this pool will send their commands.
	 */
	CommandBufferPool(const LogicalDevice& virtualGpu, QueueFamily queueFamily = QueueFamily::GRAPHICS);

	CommandBufferPool(const CommandBufferPool&) = delete;
	CommandBufferPool(CommandBufferPool&&) = default;
	CommandBufferPool& operator=(const CommandBufferPool&) = delete;
	CommandBufferPool& operator=(CommandBufferPool&&) = delete;

	~CommandBufferPool();

	const VkCommandPool& operator+() const;

private:
	VkCommandPool commandBufferPool;
	const LogicalDevice& virtualGpu;
};

#endif
