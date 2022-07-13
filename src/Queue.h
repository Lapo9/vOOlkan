#ifndef VULKAN_QUEUE
#define VULKAN_QUEUE

#include <vulkan/vulkan.h>
#include <utility>


namespace Vulkan { class Queue; class LogicalDevice; enum class QueueFamily; }

/**
 * @brief A Queue is an abstraction over the VkQueue Vulkan object. A Queue is where commands from the CPU to the GPU are sent.
 */
class Vulkan::Queue {
	public:

		/**
		 * @brief Creates a queue by getting it from the LogicalDevice.
		 * @details It is always extracted the first queue of the specified family from the LogicalDevice.
		 * 
		 * @param virtualGpu The LogicalDevice to extract the Queue from.
		 * @param queueFamilyIndex The Family of the Queue to extract
		 */
		Queue(const LogicalDevice& virtualGpu, std::pair<QueueFamily, int> queueFamilyIndex);


		/**
		 * @brief Returns the underlying VkQueue object.
		 * 
		 * @return The underlying VkQueue object.
		 */
		const VkQueue& operator+() const;

		/**
		 * @brief Returns the family of the queue.
		 * 
		 * @return The family of the queue.
		 */
		QueueFamily getFamily();


		//TODO functions to add commands to the queue

	private:
		VkQueue queue;
		QueueFamily family;
};

#endif

