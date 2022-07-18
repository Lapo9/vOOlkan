#ifndef VULKAN_WINDOW
#define VULKAN_WINDOW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <string>


namespace Vulkan { class Window; }

/**
 * @brief Manages the creation and lifetime of an OS window.
 */
class Vulkan::Window {
	public:

		/**
		 * @brief Creates a window. 
		 * @details The underlying type of the window is GLFWwindow*.
		 * 
		 * @param width Width of the window.
		 * @param height Height of the window.
		 * @param title Title of the window.
		 */
		Window(int width, int height, const std::string& title = "Vulkan window");

		Window(Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&) = delete;
		Window& operator=(Window&&) = delete;

		/**
		 * Returns the underlying Vulkan object for the window.
		 * @return The underlying Vulkan object for the window.
		 */
		GLFWwindow* operator+() const;

	private:

		// Frees up the resources of the window.
		static void deleteWindow(GLFWwindow* window);


		std::unique_ptr<GLFWwindow, decltype(&deleteWindow)> window;
		std::string title;
};

#endif
