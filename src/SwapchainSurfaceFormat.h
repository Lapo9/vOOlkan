#ifndef VULKAN_SWAPCHAINSURFACEFORMAT
#define VULKAN_SWAPCHAINSURFACEFORMAT

#include <vulkan/vulkan.h>
#include <vector>
#include <functional>


namespace Vulkan { class PhysicalDevice; class WindowSurface; namespace SwapchainOptions { class SurfaceFormat; } }

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
		SurfaceFormat(const PhysicalDevice& realGpu, const WindowSurface& windowSurface, std::function<VkSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>&)> chooseBestFormat = chooseBestFormat);



		SurfaceFormat(VkSurfaceFormatKHR format) : format{ format } {}


		/**
		 * @brief Returns the underlying VkSurfaceFormat.
		 * 
		 * @return The underlying VkSurfaceFormat.
		 */
		const VkSurfaceFormatKHR& operator+() const;


		/**
		 * Checks whether there is an available surface format for this pair of GPU and window surface.
		 * 
		 * @param realGpu The physical GPU.
		 * @param windowSurface The window surface.
		 * @return Whether there is an available surface format for this pair of GPU and window surface.
		 */
		static bool isThereAnAvailableSurfaceFormat(const PhysicalDevice& realGpu, const WindowSurface& windowSurface);

	private:

		//Returns the best format among the ones available
		static VkSurfaceFormatKHR chooseBestFormat(const std::vector<VkSurfaceFormatKHR>& formats);

		VkSurfaceFormatKHR format;
};

#endif
