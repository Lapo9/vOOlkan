#ifndef VULKAN_WINDOWSURFACE
#define VULKAN_WINDOWSURFACE

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Instance.h"
#include "Window.h"


namespace Vulkan { class WindowSurface; }

/**
 * @brief A window surface is the connection between Vulkan and the OS windows environment.
 */
class Vulkan::WindowSurface {
	public:

		WindowSurface(const Instance& vulkanInstance, const Window& window) : vulkanInstance{ vulkanInstance } {
			if (auto result = glfwCreateWindowSurface(+vulkanInstance, +window, nullptr, &surface); result != VK_SUCCESS) {
				throw VulkanException("Failed to create window surface!", result);
			}
		}

		~WindowSurface() {
			vkDestroySurfaceKHR(+vulkanInstance, surface, nullptr);
		}

		WindowSurface(const WindowSurface&) = delete;
		WindowSurface(WindowSurface&&) = delete;
		WindowSurface& operator=(const WindowSurface&) = delete;
		WindowSurface& operator=(WindowSurface&&) = delete;


		/**
		 * @brief Returns the underlying VkSurfaceKHR object.
		 *
		 * @return The underlying VkSurfaceKHR object.
		 */
		const VkSurfaceKHR& operator+() const {
			return surface;
		}

	private:
		VkSurfaceKHR surface;
		const Instance& vulkanInstance;
};

#endif
