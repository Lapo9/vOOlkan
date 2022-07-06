#ifndef VULKAN_PHYSICALDEVICE
#define VULKAN_PHYSICALDEVICE

#include <vulkan/vulkan.h>

#include "Instance.h"
#include "VulkanException.h"
#include "classes_to_do.h" //REMOVE when done


namespace Vulkan { class PhysicalDevice; }

/**
 * @brief Represents the GPU (or any other device) that will be used with Vulkan to perform computer graphics.
 */
class Vulkan::PhysicalDevice {
	public:
		PhysicalDevice(Instance vulkanInstance, WindowSurface surface) {
			pickBestDevice(vulkanInstance); //FROMHERE
		}

	private:
		void pickBestDevice(Instance vulkanInstance) {
			uint32_t gpuCount = 0;
			vkEnumeratePhysicalDevices(+vulkanInstance, &gpuCount, nullptr);
			if (gpuCount == 0) {
				throw VulkanException("Failed to find GPUs with Vulkan support!");
			}
		}

		void queueFamiliesSupport() {

		}

		void extensionsSupport() {

		}

		VkPhysicalDevice gpu;
};

#endif
