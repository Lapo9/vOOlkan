#include "SwapchainSurfaceFormat.h"
#include "PhysicalDevice.h"
#include "WindowSurface.h"
#include "VulkanException.h"


Vulkan::SwapchainOptions::SurfaceFormat::SurfaceFormat(const PhysicalDevice& realGpu, const WindowSurface& windowSurface, std::function<VkSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>&)> chooseBestFormat) {
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



const VkSurfaceFormatKHR& Vulkan::SwapchainOptions::SurfaceFormat::operator+() const {
	return format;
}



bool Vulkan::SwapchainOptions::SurfaceFormat::isThereAnAvailableSurfaceFormat(const PhysicalDevice& realGpu, const WindowSurface& windowSurface) {
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(+realGpu, +windowSurface, &formatCount, nullptr);
	return formatCount != 0;
}



VkSurfaceFormatKHR Vulkan::SwapchainOptions::SurfaceFormat::chooseBestFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
	for (const auto& format : formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}
	return formats[0]; //if no format has the desired properties, settle down with the first format
}