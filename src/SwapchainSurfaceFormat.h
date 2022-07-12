#ifndef VULKAN_SWAPCHAINSURFACEFORMAT
#define VULKAN_SWAPCHAINSURFACEFORMAT

#include <vulkan/vulkan.h>
#include <vector>
#include <functional>

#include "PhysicalDevice.h"
#include "WindowSurface.h"

namespace Vulkan::SwapchainOptions { class SurfaceFormat; }

/**
 * @brief The surface format defines the properties of the image where we will draw, such as the color space.
 */
class Vulkan::SwapchainOptions::SurfaceFormat {
	public:

		/**
		 * @brief Creates a SurfaceFormat object containing the best VkSurfaceFormatKHR for this pair of physical GPU and window surface.
		 * @details The default function to choose the best format among the ones available will choose a format which supports VK_COLOR_SPACE_SRGB_NONLINEAR_KHR color space and VK_FORMAT_B8G8R8A8_SRGB format. If this is not available it will return a random format available.
		 * 
		 * @param realGpu The Physical GPU.
		 * @param windowSurface The OS window surface.
		 * @param chooseBestFormat Optional function to choose the best format among the ones available.
		 */
		SurfaceFormat(const PhysicalDevice& realGpu, const WindowSurface& windowSurface, std::function<VkSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>&)> chooseBestFormat = chooseBestFormat) {
			//obtain all of the available formats
			std::vector<VkSurfaceFormatKHR> formats;
			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(+realGpu, +windowSurface, &formatCount, nullptr);
			if (formatCount != 0) {
				formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(+realGpu, +windowSurface, &formatCount, formats.data());
			}
			else {
				throw VulkanException("No surface formats for the swapchain available");
			}

			//choose the best format
			format = chooseBestFormat(formats);
		}


		/**
		 * @brief Returns the underlying VkSurfaceFormat.
		 * 
		 * @return The underlying VkSurfaceFormat.
		 */
		const VkSurfaceFormatKHR& operator+() const {
			return format;
		}


		/**
		 * Checks whether there is an available surface format for this pair of GPU and window surface.
		 * 
		 * @param realGpu The physical GPU.
		 * @param windowSurface The window surface.
		 * @return Whether there is an available surface format for this pair of GPU and window surface.
		 */
		static bool isThereAnAvailableSurfaceFormat(const PhysicalDevice& realGpu, const WindowSurface& windowSurface) {
			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(+realGpu, +windowSurface, &formatCount, nullptr);
			return formatCount != 0;
		}

	private:

		//Returns the best format among the ones available
		static VkSurfaceFormatKHR chooseBestFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
			for (const auto& format : formats) {
				if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
					return format;
				}
			}
			return formats[0]; //if no format has the desired properties, settle down with the first format
		}

		VkSurfaceFormatKHR format;
};

#endif
