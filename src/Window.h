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
		Window(int width, int height, const std::string& title = "Vulkan window") {
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //TODO make it resizable in the future
			window = std::unique_ptr<GLFWwindow, decltype(deleteWindow)>(glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr), &deleteWindow);
		}

		/**
		 * Returns the underlying Vulkan object for the window.
		 * @return The underlying Vulkan object for the window.
		 */
		const GLFWwindow* operator+() const {
			return window.get();
		}

	private:

		// Frees up the resources of the window.
		void deleteWindow(GLFWwindow* window) {
			glfwDestroyWindow(window);
		}

		std::unique_ptr<GLFWwindow, decltype(deleteWindow)> window;
};

#endif
