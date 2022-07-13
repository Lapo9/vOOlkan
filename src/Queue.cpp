#include "Queue.h"
#include "LogicalDevice.h"


Vulkan::Queue::Queue(const LogicalDevice& virtualGpu, std::pair<QueueFamily, int> queueFamilyIndex) : family{ queueFamilyIndex.first } {
	vkGetDeviceQueue(+virtualGpu, queueFamilyIndex.second, 0, &queue); //get the queue from the device
}



const VkQueue& Vulkan::Queue::operator+() const {
	return queue;
}



Vulkan::QueueFamily Vulkan::Queue::getFamily() {
	return family;
}
