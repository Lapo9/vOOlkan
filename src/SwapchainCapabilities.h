#ifndef VULKAN_SWAPCHAINCAPABILITIES
#define VULKAN_SWAPCHAINCAPABILITIES

#include <vulkan/vulkan.h>
#include <algorithm>

#include "PhysicalDevice.h"
#include "WindowSurface.h"
#include "Window.h"


namespace Vulkan::SwapchainOptions { class Capabilities; }

/**
 * @brief The capabilities of a swapchain define some important properties of the images in the swapchain, such as their resolution.
 */
class Vulkan::SwapchainOptions::Capabilities {
	public:

        /**
         * @brief Creates an object containing the capabilities of the pair GPU window surface, such has the maximum resolution.
         * 
         * @param realGpu The physical GPU.
         * @param windowSurface The OS window surface.
         */
		Capabilities(const PhysicalDevice& realGpu, const WindowSurface& windowSurface) {
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(+realGpu, +windowSurface, &capabilities);
		}


        /**
         * @brief Returns the underlying VkSurfaceCapabilitiesKHR.
         *
         * @return The underlying VkSurfaceCapabilitiesKHR.
         */
        const VkSurfaceCapabilitiesKHR& operator+() const {
            return capabilities;
        }


        /**
         * @brief Chooses the best swap extent (basically the resolution of the images to be shown) based on the window and the capabilities of the GPU and the window surface.
         * 
         * @param window Current OS window.
         * @return The best swap extent for this window considering the capabilities of the GPU and the window surface.
         */
        VkExtent2D chooseSwapExtent(const Window& window) const {
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

	private:
		VkSurfaceCapabilitiesKHR capabilities;
};

#endif