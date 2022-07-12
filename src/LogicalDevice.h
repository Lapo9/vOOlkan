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
			for (float queuePriority = 1.0f; const auto& queueFamilyIndex : queueFamiliesIndices) {
				//struct to create a queue
				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamilyIndex.second;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;
				queueCreateInfos.push_back(queueCreateInfo); //add the struct to the list
			}

			
			VkPhysicalDeviceFeatures deviceFeatures{}; //advanced features we need (nothing at the moment)
			//struct containing info for the creation of the logical device based on the choosen physical device
			VkDeviceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.queueCreateInfoCount = queueCreateInfos.size();
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.pEnabledFeatures = &deviceFeatures;
			createInfo.enabledExtensionCount = physicalGpu.getRequiredDeviceExtensions().size();
			createInfo.ppEnabledExtensionNames = physicalGpu.getRequiredDeviceExtensions().data();

			//actually create the virtual GPU
			if (VkResult result = vkCreateDevice(+physicalGpu, &createInfo, nullptr, &virtualGpu); result != VK_SUCCESS) {
				throw VulkanException("Failed to create logical device!", result);
			}

			//save the queues so they are accessible later on
			for (const auto& queueFamilyIndex : queueFamiliesIndices) {
				queues.emplace(queueFamilyIndex.first, Queue(*this, queueFamilyIndex)); //create and insert the Queue object into the list of queues
			}
		}

		~LogicalDevice() {
			vkDestroyDevice(virtualGpu, nullptr);
		}

		LogicalDevice(const LogicalDevice&) = delete;
		LogicalDevice(LogicalDevice&&) = delete;
		LogicalDevice& operator=(const LogicalDevice&) = delete;
		LogicalDevice& operator=(LogicalDevice&&) = delete;


		/**
		 * @brief Returns a const reference to the underlying Vulkan VkDevice object.
		 * 
		 * @return The underlying Vulkan VkDevice object
		 */
		const VkDevice& operator+() const {
			return virtualGpu;
		}


		/**
		 * @brief Returns the queue of the specified queue family.
		 * 
		 * @param queueFamily The queue family of the queue to return.
		 * @return The queue of the specified queue family.
		 */
		const Queue& operator[](QueueFamily queueFamily) {
			return queues[queueFamily];
		}


	private:
		VkDevice virtualGpu;
		std::map<QueueFamily, Queue> queues; //the queues we created
		
};


#endif