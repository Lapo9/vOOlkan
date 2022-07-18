#ifndef VULKAN_LOGICALDEVICE
#define VULKAN_LOGICALDEVICE

#include <vulkan/vulkan.h>
#include <vector>
#include <map>



namespace Vulkan { class LogicalDevice; class PhysicalDevice; enum class QueueFamily; class Queue; }

/**
 * @brief A logical device is an abstraction of the physical GPU which we can mainly use to send commands.
 */
class Vulkan::LogicalDevice {
	public:
		LogicalDevice(const PhysicalDevice& physicalGpu);

		~LogicalDevice();

		LogicalDevice(const LogicalDevice&) = delete;
		LogicalDevice(LogicalDevice&&) = delete;
		LogicalDevice& operator=(const LogicalDevice&) = delete;
		LogicalDevice& operator=(LogicalDevice&&) = delete;


		/**
		 * @brief Returns a const reference to the underlying Vulkan VkDevice object.
		 * 
		 * @return The underlying Vulkan VkDevice object
		 */
		const VkDevice& operator+() const;


		/**
		 * @brief Returns the queue of the specified queue family.
		 * 
		 * @param queueFamily The queue family of the queue to return.
		 * @return The queue of the specified queue family.
		 */
		const Queue& operator[](QueueFamily queueFamily) const;


	private:
		VkDevice virtualGpu;
		std::map<QueueFamily, Queue> queues; //the queues we created
		
};


#endif