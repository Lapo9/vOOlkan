#ifndef VULKAN_LOGICALDEVICE
#define VULKAN_LOGICALDEVICE

#include <vulkan/vulkan.h>
#include <vector>
#include <ranges>

#include "VulkanException.h"
#include "PhysicalDevice.h"


namespace Vulkan { class LogicalDevice; }

class Vulkan::LogicalDevice {
	public:
		LogicalDevice(const PhysicalDevice& physicalGpu) {
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

			
			VkPhysicalDeviceFeatures deviceFeatures{}; // advanced features we need (nothing at the moment)
			//struct containing info for the creation of the logical device based on the choosen physical device
			VkDeviceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.queueCreateInfoCount = queueCreateInfos.size();
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.pEnabledFeatures = &deviceFeatures;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			createInfo.ppEnabledExtensionNames = deviceExtensions.data();

			//actually create the virtual GPU
			if (VkResult result = vkCreateDevice(+physicalGpu, &createInfo, nullptr, &virtualGpu); result != VK_SUCCESS) {
				throw VulkanException("Failed to create logical device!", result);
			}

			//FROMHERE save the queues somewhere
		}

		~LogicalDevice() {
			vkDestroyDevice(virtualGpu, nullptr);
		}

		LogicalDevice(const LogicalDevice&) = delete;
		LogicalDevice(LogicalDevice&&) = delete;
		LogicalDevice& operator=(const LogicalDevice&) = delete;
		LogicalDevice& operator=(LogicalDevice&&) = delete;

		const VkDevice& operator+() {
			return virtualGpu;
		}

	private:
		VkDevice virtualGpu;
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME }; //FIXTHIS we must move this to the PhysicalDevice class
};


#endif