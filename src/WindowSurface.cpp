#include "WindowSurface.h"
#include "Window.h"
#include "Instance.h"
#include "VulkanException.h"

#include <iostream>


Vulkan::WindowSurface::WindowSurface(const Instance& vulkanInstance, const Window& window) : vulkanInstance{ vulkanInstance } {
	if (auto result = glfwCreateWindowSurface(+vulkanInstance, +window, nullptr, &surface); result != VK_SUCCESS) {
		std::string hint = result == -3 ? "Make sure to call glfwInit() before any call to any other GLFW function."
			: result == -7 ? "Make sure the Window is created before the Vulkan Instance" : "";
		throw VulkanException("Failed to create window surface!", result, hint);
	}

	std::cout << "\n+ WindowSurface created";
}



Vulkan::WindowSurface::~WindowSurface() {
	vkDestroySurfaceKHR(+vulkanInstance, surface, nullptr);
	std::cout << "\n- WindowSurface destroyed";
}



const VkSurfaceKHR& Vulkan::WindowSurface::operator+() const {
	return surface;
}
