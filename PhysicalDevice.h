#ifndef VULKAN_PHYSICALDEVICE
#define VULKAN_PHYSICALDEVICE

#include <vulkan/vulkan.h>

#include "src/Instance.h"
#include "VulkanException.h"
#include "classes_to_do.h"


namespace Vulkan { class PhysicalDevice; }

/**
 * @brief Represents the GPU (or any other device) that will be used with Vulkan to perform computer graphics.
 */
class PhysicalDevice {
	public:
		PhysicalDevice(Instance vulkanInstance, WindowSurface surface) {
			pickBestDevice();
		}

	private:
		void pickBestDevice() {

		}

		void queueFamiliesSupport() {

		}

		void extensionsSupport() {

		}

		VkPhysicalDevice gpu;
};

#endif
