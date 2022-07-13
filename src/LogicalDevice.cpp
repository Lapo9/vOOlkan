#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Queue.h"
#include "VulkanException.h"

#include <iostream>


Vulkan::LogicalDevice::LogicalDevice(const PhysicalDevice& physicalGpu) {
	const auto queueFamiliesIndices = physicalGpu.getQueueFamiliesIndices(); //indices of the queues families for the graphics and presentation queues
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos; //array where to save the structs to create the queues

	//for each queue family (graphics and presentation) create the concrete queue
	for (float queuePriority = 1.0f; const auto & queueFamilyIndex : queueFamiliesIndices) {
		//struct to create a queue
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex.second;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo); //add the struct to the list
	}


	VkPhysicalDeviceFeatures deviceFeatures{}; //advanced features we need (nothing at the moment)
	auto requiredDeviceExtensions = physicalGpu.getRequiredDeviceExtensions();
	//struct containing info for the creation of the logical device based on the choosen physical device
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

	//actually create the virtual GPU
	if (VkResult result = vkCreateDevice(+physicalGpu, &createInfo, nullptr, &virtualGpu); result != VK_SUCCESS) {
		throw VulkanException("Failed to create logical device!", result);
	}

	//save the queues so they are accessible later on
	for (const auto& queueFamilyIndex : queueFamiliesIndices) {
		queues.emplace(std::piecewise_construct, 
			std::forward_as_tuple(queueFamilyIndex.first), 
			std::forward_as_tuple(*this, queueFamilyIndex)); //create and insert the Queue object into the list of queues
	}

	std::cout << "\n+ LogicalDevice created";
}



Vulkan::LogicalDevice::~LogicalDevice() {
	vkDestroyDevice(virtualGpu, nullptr);
}



const VkDevice& Vulkan::LogicalDevice::operator+() const {
	return virtualGpu;
}



const Vulkan::Queue& Vulkan::LogicalDevice::operator[](QueueFamily queueFamily) {
	return queues.find(queueFamily)->second;
}
