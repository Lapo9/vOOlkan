#ifndef VULKAN_SWAPCHAIN
#define VULKAN_SWAPCHAIN

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>

#include "SwapchainCapabilities.h"
#include "SwapchainSurfaceFormat.h"
#include "SwapchainPresentMode.h"


namespace Vulkan { class Swapchain; class PhysicalDevice; class LogicalDevice; class WindowSurface; class Window; class Image; }

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
		Swapchain(const PhysicalDevice& realGpu, const LogicalDevice& virtualGpu, const WindowSurface& windowSurface, const Window& window);

		~Swapchain();

		Swapchain(const Swapchain&) = delete;
		Swapchain(Swapchain&&) = default;
		Swapchain& operator=(const Swapchain&) = delete;
		Swapchain& operator=(Swapchain&&) = default;

		/**
		 * @brief Returns the underlying VkSwapchainKHR object.
		 * 
		 * @return The underlying VkSwapchainKHR object.
		 */
		const VkSwapchainKHR& operator+();


		const SwapchainOptions::SurfaceFormat& getImageFormat();

		const SwapchainOptions::Capabilities& getSwapchainCapabilities();

		/**
		 * @brief Returns the images of this swapchain.
		 * 
		 * @return The images of this swapchain.
		 */
		const std::vector<Image>& getImages() const;


		/**
		 * @brief Returns the width and height of the images in this swapchain.
		 * 
		 * @return The width and height of the images in this swapchain.
		 */
		std::pair<unsigned int, unsigned int> getResolution() const;
		


		/**
		 * @brief Checks whether there exist a swapchain for this physiscal GPU and windows surface.
		 * 
		 * @param realGpu The physiscal GPU.
		 * @param windowSurface The window surface.
		 * @return Whether the swapchain is supported.
		 */
		static bool isSwapchainSupported(const PhysicalDevice& realGpu, const WindowSurface& windowSurface);

	private:

		//Saves the images of the swap chain to be able to access them later on
		void saveSwapchainImages();


		VkSwapchainKHR swapchain;
		SwapchainOptions::Capabilities swapchainCapabilities;
		SwapchainOptions::SurfaceFormat swapchainSurfaceFormat;
		SwapchainOptions::PresentMode swapchainPresentMode;

		std::vector<Image> images; //the images which the swapchain holds
		const LogicalDevice& virtualGpu;
};

#endif
