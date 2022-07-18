#include "Queue.h"
#include "LogicalDevice.h"

#include <iostream>


Vulkan::Queue::Queue(const LogicalDevice& virtualGpu, std::pair<QueueFamily, int> queueFamilyIndex) : family{ queueFamilyIndex.first }, queueFamilyIndex{ queueFamilyIndex.second } {
	vkGetDeviceQueue(+virtualGpu, queueFamilyIndex.second, 0, &queue); //get the queue from the device
	std::cout << "\n+ Queue created";
}



const VkQueue& Vulkan::Queue::operator+() const {
	return queue;
}



Vulkan::QueueFamily Vulkan::Queue::getFamily() const {
	return family;
}



int Vulkan::Queue::getFamilyIndex() const {
	return queueFamilyIndex;
}
