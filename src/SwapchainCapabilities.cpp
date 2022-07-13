#include "SwapchainCapabilities.h"
#include "Window.h"
#include "PhysicalDevice.h"
#include "WindowSurface.h"


Vulkan::SwapchainOptions::Capabilities::Capabilities(const PhysicalDevice& realGpu, const WindowSurface& windowSurface) {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(+realGpu, +windowSurface, &capabilities);
}


const VkSurfaceCapabilitiesKHR& Vulkan::SwapchainOptions::Capabilities::operator+() const {
    return capabilities;
}


VkExtent2D Vulkan::SwapchainOptions::Capabilities::chooseSwapExtent(const Window& window) const {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(+window, &width, &height);
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
