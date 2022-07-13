#ifndef VULKAN_SWAPCHAINPRESENTMODE
#define VULKAN_SWAPCHAINPRESENTMODE

#include <vulkan/vulkan.h>
#include <vector>
#include <functional>


namespace Vulkan { class PhysicalDevice; class WindowSurface; namespace SwapchainOptions { class PresentMode; } }

/**
 * @brief The present mode defines how the images of the swapchain are sent to the screen.
 * @details An object of this class will qury which present modes are available based on the GPU and window surface, and will chose the best one.
 */
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
		PresentMode(const PhysicalDevice& realGpu, const WindowSurface& windowSurface, std::function<VkPresentModeKHR(const std::vector<VkPresentModeKHR>&)> chooseBestPresentMode = chooseBestPresentMode);


		/**
		 * @brief Returns the underlying VkPresentModeKHR.
		 *
		 * @return The underlying VkPresentModeKHR.
		 */
		const VkPresentModeKHR& operator+() const;


		/**
		 * Checks whether there is an available present mode for this pair of GPU and window surface.
		 *
		 * @param realGpu The physical GPU.
		 * @param windowSurface The window surface.
		 * @return Whether there is an available present mode for this pair of GPU and window surface.
		 */
		static bool isThereAnAvailablePresentMode(const PhysicalDevice& realGpu, const WindowSurface& windowSurface);

	private:

		//chooses the best present mode among the ones available
		static VkPresentModeKHR chooseBestPresentMode(const std::vector<VkPresentModeKHR>& presentModes);

		VkPresentModeKHR presentMode;
};

#endif
