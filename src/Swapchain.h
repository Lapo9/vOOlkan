#ifndef VULKAN_SWAPCHAIN
#define VULKAN_SWAPCHAIN

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>

#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "Window.h"
#include "SwapchainCapabilities.h"
#include "SwapchainPresentMode.h"
#include "SwapchainSurfaceFormat.h"
#include "VulkanException.h"
#include "classes_to_do.h"


namespace Vulkan { class Swapchain; }

/**
 * @brief Object which holds images to be presented to the WindowSurface.
 */
class Vulkan::Swapchain {
	public:
		
		/**
		 * @brief the best swapchain possible to fit with the current GPU and window.
		 * 
		 * @param realGpu The physical GPU.
		 * @param virtualGpu The logical GPU.
		 * @param windowSurface The surface where images will be drawn.
		 * @param window The OS window where the Vulkan application is running.
		 */
		Swapchain(const PhysicalDevice& realGpu, const LogicalDevice& virtualGpu, const WindowSurface& windowSurface, const Window& window) :
			swapchainCapabilities{ realGpu, windowSurface },
			swapchainSurfaceFormat{ realGpu, windowSurface },
			swapchainPresentMode{ realGpu, windowSurface },
			virtualGpu{ virtualGpu }
		{
			//struct to create the swapchain with the specified properties
			VkSwapchainCreateInfoKHR createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.surface = +windowSurface;
			createInfo.minImageCount = std::clamp((+swapchainCapabilities).minImageCount + 1, (+swapchainCapabilities).minImageCount, (+swapchainCapabilities).maxImageCount); //we want 1 more image in the swap chain than the minimum in order not to wait if all images are occupied
			createInfo.imageFormat = (+swapchainSurfaceFormat).format;
			createInfo.imageColorSpace = (+swapchainSurfaceFormat).colorSpace;
			createInfo.imageExtent = swapchainCapabilities.chooseSwapExtent(window);
			createInfo.imageArrayLayers = 1;
			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			createInfo.preTransform = (+swapchainCapabilities).currentTransform;
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			createInfo.presentMode = +swapchainPresentMode;
			createInfo.clipped = VK_TRUE;
			createInfo.oldSwapchain = VK_NULL_HANDLE;

			//if the queue families for the graphics and presentation queues are different, then whe have to use a particular option
			uint32_t queueFamilyIndices[] = { realGpu.getQueueFamiliesIndices()[QueueFamily::GRAPHICS], realGpu.getQueueFamiliesIndices()[QueueFamily::PRESENTATION] };
			if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
				createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				createInfo.queueFamilyIndexCount = 2;
				createInfo.pQueueFamilyIndices = queueFamilyIndices;
			}
			else {
				createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				createInfo.queueFamilyIndexCount = 0; // Optional
				createInfo.pQueueFamilyIndices = nullptr; // Optional
			}

			//actually create the swapchain with the specified options
			if (auto result = vkCreateSwapchainKHR(+virtualGpu, &createInfo, nullptr, &swapchain); result != VK_SUCCESS) {
				throw VulkanException("Failed to create swap chain!", result);
			}
		}

		~Swapchain() {
			vkDestroySwapchainKHR(+virtualGpu, swapchain, nullptr);
		}

		Swapchain(const Swapchain&) = delete;
		Swapchain(Swapchain&&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;
		Swapchain& operator=(Swapchain&&) = delete;

		/**
		 * @brief Returns the underlying VkSwapchainKHR object.
		 * 
		 * @return The underlying VkSwapchainKHR object.
		 */
		const VkSwapchainKHR& operator+() {
			return swapchain;
		}


		/**
		 * @brief Checks whether there exist a swapchain for this physiscal GPU and windows surface.
		 * 
		 * @param realGpu The physiscal GPU.
		 * @param windowSurface The window surface.
		 * @return Whether the swapchain is supported.
		 */
		static bool isSwapchainSupported(const PhysicalDevice& realGpu, const WindowSurface& windowSurface) {
			return SwapchainOptions::SurfaceFormat::isThereAnAvailableSurfaceFormat(realGpu, windowSurface) && SwapchainOptions::PresentMode::isThereAnAvailablePresentMode(realGpu, windowSurface);
		}

	private:
		VkSwapchainKHR swapchain;
		SwapchainOptions::Capabilities swapchainCapabilities;
		SwapchainOptions::SurfaceFormat swapchainSurfaceFormat;
		SwapchainOptions::PresentMode swapchainPresentMode;

		std::vector<Image> images; //the images which the swapchain holds
		const LogicalDevice& virtualGpu;
};

#endif
