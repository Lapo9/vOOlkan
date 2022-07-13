#ifndef VULKAN_WINDOWSURFACE
#define VULKAN_WINDOWSURFACE

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace Vulkan { class WindowSurface; class Window; class Instance; }

/**
 * @brief A window surface is the connection between Vulkan and the OS windows environment.
 */
class Vulkan::WindowSurface {
	public:

		WindowSurface(const Instance& vulkanInstance, const Window& window);

		~WindowSurface();

		WindowSurface(const WindowSurface&) = delete;
		WindowSurface(WindowSurface&&) = delete;
		WindowSurface& operator=(const WindowSurface&) = delete;
		WindowSurface& operator=(WindowSurface&&) = delete;


		/**
		 * @brief Returns the underlying VkSurfaceKHR object.
		 *
		 * @return The underlying VkSurfaceKHR object.
		 */
		const VkSurfaceKHR& operator+() const;

	private:
		VkSurfaceKHR surface;
		const Instance& vulkanInstance;
};

#endif
