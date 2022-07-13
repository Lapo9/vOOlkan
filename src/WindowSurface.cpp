#include "WindowSurface.h"
#include "Window.h"
#include "Instance.h"
#include "VulkanException.h"

#include <iostream>


Vulkan::WindowSurface::WindowSurface(const Instance& vulkanInstance, const Window& window) : vulkanInstance{ vulkanInstance } {
	if (auto result = glfwCreateWindowSurface(+vulkanInstance, +window, nullptr, &surface); result != VK_SUCCESS) {
		const char* descr;
		auto err = glfwGetError(&descr);
		throw VulkanException("Failed to create window surface!", result);
	}

	std::cout << "\n+ WindowSurface created";
}



Vulkan::WindowSurface::~WindowSurface() {
	vkDestroySurfaceKHR(+vulkanInstance, surface, nullptr);
}



const VkSurfaceKHR& Vulkan::WindowSurface::operator+() const {
	return surface;
}
