#ifndef VULKAN_SWAPCHAINPRESENTMODE
#define VULKAN_SWAPCHAINPRESENTMODE

#include <vulkan/vulkan.h>
#include <vector>
#include <functional>

#include "PhysicalDevice.h"
#include "WindowSurface.h"


namespace Vulkan::SwapchainOptions { class PresentMode; }

class Vulkan::SwapchainOptions::PresentMode {
	public:

		/**
		 * @brief Creates a PresentMode object containing the best VkPresentModeKHR for this pair of physical GPU and window surface.
		 * @details The default function to choose the best present mode among the ones available will choose a present mode with VK_PRESENT_MODE_MAILBOX_KHR. If this is not available it will return a random present mode available.
		 *
		 * @param realGpu The Physical GPU.
		 * @param windowSurface The OS window surface.
		 * @param chooseBestFormat Optional function to choose the best present mode among the ones available.
		*/
		PresentMode(const PhysicalDevice& realGpu, const WindowSurface& windowSurface, std::function<VkPresentModeKHR(const std::vector<VkPresentModeKHR>&)> chooseBestPresentMode = chooseBestPresentMode) {
			//obtain all of the available present modes
			std::vector<VkPresentModeKHR> presentModes;
			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(+realGpu, +windowSurface, &presentModeCount, nullptr);
			if (presentModeCount != 0) {
				presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(+realGpu, +windowSurface, &presentModeCount, presentModes.data());
			}
			else {
				throw VulkanException("No present modes for the swapchain available");
			}

			//choose the best present mode
			presentMode = chooseBestPresentMode(presentModes);
		}


		/**
		 * @brief Returns the underlying VkPresentModeKHR.
		 *
		 * @return The underlying VkPresentModeKHR.
		 */
		const VkPresentModeKHR& operator+() const {
			return presentMode;
		}


		/**
		 * Checks whether there is an available present mode for this pair of GPU and window surface.
		 *
		 * @param realGpu The physical GPU.
		 * @param windowSurface The window surface.
		 * @return Whether there is an available present mode for this pair of GPU and window surface.
		 */
		static bool isThereAnAvailablePresentMode(const PhysicalDevice& realGpu, const WindowSurface& windowSurface) {
			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(+realGpu, +windowSurface, &presentModeCount, nullptr);
			return presentModeCount != 0;
		}

	private:

		//chooses the best present mode among the ones available
		static VkPresentModeKHR chooseBestPresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
			for (const auto& presentMode : presentModes) {
				if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
					return presentMode;
				}
			}

			return VK_PRESENT_MODE_FIFO_KHR; //if no present mode with VK_PRESENT_MODE_MAILBOX_KHR has been fount, return the first present mode
		}

		VkPresentModeKHR presentMode;
};

#endif
