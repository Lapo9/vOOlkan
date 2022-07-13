#include "SwapchainPresentMode.h"
#include "PhysicalDevice.h"
#include "WindowSurface.h"
#include "VulkanException.h"


Vulkan::SwapchainOptions::PresentMode::PresentMode(const PhysicalDevice& realGpu, const WindowSurface& windowSurface, std::function<VkPresentModeKHR(const std::vector<VkPresentModeKHR>&)> chooseBestPresentMode) {
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



const VkPresentModeKHR& Vulkan::SwapchainOptions::PresentMode::operator+() const {
	return presentMode;
}



bool Vulkan::SwapchainOptions::PresentMode::isThereAnAvailablePresentMode(const PhysicalDevice& realGpu, const WindowSurface& windowSurface) {
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(+realGpu, +windowSurface, &presentModeCount, nullptr);
	return presentModeCount != 0;
}



VkPresentModeKHR Vulkan::SwapchainOptions::PresentMode::chooseBestPresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
	for (const auto& presentMode : presentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return presentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR; //if no present mode with VK_PRESENT_MODE_MAILBOX_KHR has been fount, return the first present mode
}